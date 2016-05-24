#include "JSound.h"
#include "JSoundLoader.h"
#include "JWaveLoader.h"
#include "JUtility.h"
#include "JVoice.h"
#include <map>
#include <functional>
#include "JApplication.h"
#include "JSoundPlayer.h"

using namespace Javelin;

//
// CSound
//

std::list<CSound*> CSound::soundList;

CSound::CSound() :
m_ploader(nullptr),
m_volume(1.0f),
m_pitch(1.0f){
	soundList.push_back(this);
}

CSound::~CSound() {
	auto it = soundList.begin();
	while (it != soundList.end()) {
		if (*it == this) {
			it = soundList.erase(it);
		} else {
			it++;
		}
	}
}

namespace {
	const std::map<std::string, std::function<CSoundLoader*()>> loaderList = {
		std::make_pair("wav", [] {return new CWaveLoader; }),
	};

	const std::string extensionList[SF_DUMMY] = {
		"",
		"wav",
	};
}

CSoundLoader* CSound::createLoader(std::string extension) {
	if (loaderList.find(extension) == loaderList.end()) {
		return nullptr;
	}
	return (loaderList.at(extension))();
}

void CSound::Initialize(std::string filename, DWORD loopPos, SOUNDFORMAT format) {
	std::vector<std::string> splitedStr;
	Split(filename, splitedStr, '.');
	if (splitedStr.size() < 2) {
		Application::WriteLog("CSound::Initialize ファイル名が不正です：" + filename);
		throw - 1;
	}
	auto ptr = createLoader(format != SF_DEFAULT ? extensionList[format] : *splitedStr.rbegin());
	if (!ptr) {
		Application::WriteLog("CSound::Initialize ファイル名が不正です：" + filename);
		throw - 1;
	}
	m_ploader.reset(ptr);
	m_ploader->Initialize(filename, loopPos);
}

CSound::operator bool() const noexcept {
	return m_ploader && m_ploader->operator bool();
}

DWORD CSound::GetLoopPos() const {
	if (!m_ploader) {
		return 0;
	}
	const auto& format = m_ploader->GetFormat();
	DWORD loopPos = format.nSamplesPerSec * m_ploader->GetLoopPos() / 1000;
	if (m_ploader->GetLoopPos() * format.nAvgBytesPerSec > m_ploader->GetSize() * 1000) {
		loopPos = 0;
	}
	return loopPos;
}

HRESULT CSound::play_sub(IXAudio2* audio2, const XAUDIO2_BUFFER* buffer, DWORD size, 
						 DWORD* sizeSubmit, std::shared_ptr<CVoice>* ppVoice) const {
	if (!operator bool() || !SoundPlayer::IsSoundAvailable()) {
		return (E_FAIL);
	}

	IXAudio2SourceVoice* pSourceVoice;
	HRESULT hr = audio2->CreateSourceVoice(&pSourceVoice, &m_ploader->GetFormat());
	if (FAILED(hr)) {
		return (hr);
	}

	if (m_pWaveData[0].empty()) {
		m_pWaveData[0].resize((size_t)size);
		m_ploader->Read(m_pWaveData[0].data(), 0, size, &size);
	}

	XAUDIO2_BUFFER buf = *buffer;
	buf.pAudioData = m_pWaveData[0].data();
	buf.AudioBytes = size;

	hr = pSourceVoice->SubmitSourceBuffer(&buf);
	if (FAILED(hr)) {
		pSourceVoice->DestroyVoice();
		return (hr);
	}

	if (sizeSubmit) {
		*sizeSubmit = size;
	}

	auto voice = std::make_shared<CVoice>(pSourceVoice, buf, GetLoopPos(), m_ploader->GetFormat().nChannels);
	m_voiceList.push_back(voice);

	if (ppVoice) {
		*ppVoice = voice;
	}

	hr = pSourceVoice->Start(0);
	if (FAILED(hr)) {
		return (hr);
	}

	voice->SetVolume(m_volume);
	voice->SetFrequencyRatio(m_pitch);

	return hr;
}

void CSound::MainLoop() {
	auto it = m_voiceList.begin();
	while (it != m_voiceList.end()) {
		if (it->use_count() <= 1 && !(*it)->IsRunnning() && !(*it)->IsPaused()) {
			it = m_voiceList.erase(it);
		} else {
			(*it)->Polling();
			it++;
		}
	}
}

HRESULT CSound::play(IXAudio2* audio2, const XAUDIO2_BUFFER* buffer, std::shared_ptr<CVoice>* ppVoice) const {
	return play_sub(audio2, buffer, m_ploader->GetSize(), nullptr, ppVoice);
}

HRESULT CSound::pause() const {
	for (auto it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		(*it)->Pause();
	}
	return S_OK;
}

HRESULT CSound::resume() const {
	for (auto it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		(*it)->Resume();
	}
	return S_OK;
}

HRESULT CSound::stop() const {
	for (auto it = m_voiceList.begin(); it != m_voiceList.end(); it++) {
		(*it)->Stop();
	}
	return S_OK;
}

void CSound::SetVolume(float volume) const {
	m_volume = volume;
	m_volume = max(m_volume, -1.0f);
	m_volume = min(m_volume, 1.0f);
	for (auto it : m_voiceList) {
		it->SetVolume(m_volume);
	}
}

void CSound::SetFrequencyRatio(float ratio) const {
	m_pitch = ratio;
	m_pitch = max(m_pitch, 0.0f);
	m_pitch = min(m_pitch, XAUDIO2_DEFAULT_FREQ_RATIO);
	for (auto it : m_voiceList) {
		it->SetFrequencyRatio(m_pitch);
	}
}

void CSound::MainLoopAll() {
	for (auto it : soundList) {
		it->MainLoop();
	}
}

//
// CSoundStreaming
//

CSoundStreaming::CSoundStreaming() :
m_division_size(1048576),
m_currentPos(0),
m_index(0){
}

CSoundStreaming::~CSoundStreaming() {
}

void CSoundStreaming::Initialize(std::string filename, UINT division_size,
				   DWORD loopPos, SOUNDFORMAT format) {
	m_division_size = division_size;
	if (m_division_size % 4 != 0) {
		m_division_size = ceil_to_multiple<UINT>(m_division_size, 4);
		Application::WriteLog(("ストリーミングサウンドの分割数は4の倍数である必要があります。分割数を" 
			+ std::to_string(m_division_size) + "に強制します").c_str());
	}
	CSound::Initialize(filename, loopPos, format);
}

HRESULT CSoundStreaming::play(IXAudio2* audio2, const XAUDIO2_BUFFER* buffer, std::shared_ptr<CVoice>* ppVoice) const {
	m_loopCount = buffer->LoopCount;

	auto buf = *buffer;
	buf.LoopCount = 0;
	buf.LoopBegin = 0;
	buf.Flags = 0;

	m_index = 1;
	m_currentLoopCount = 0;

	m_pWaveData[0].clear();
	if (m_pVoice) {
		m_pVoice->Stop();
	}
	m_pVoice.reset();
	auto hr = play_sub(audio2, &buf, m_division_size, &m_currentPos, &m_pVoice);
	if (ppVoice) {
		*ppVoice = m_pVoice;
	}
	return hr;
}

HRESULT CSoundStreaming::pause() const {
	if (!m_pVoice) {
		return S_FALSE;
	}
	return m_pVoice->Pause();
}

HRESULT CSoundStreaming::resume() const {
	if (!m_pVoice) {
		return S_FALSE;
	}
	return m_pVoice->Resume();
}

HRESULT CSoundStreaming::stop() const{
	if (!m_pVoice) {
		return S_FALSE;
	}
	m_pVoice->Stop();
	m_pVoice.reset();

	return S_OK;
}

void CSoundStreaming::MainLoop() {
	CSound::MainLoop();
	Polling();
}

HRESULT CSoundStreaming::Polling() {
	if (!m_pVoice) {
		return S_FALSE;
	}

	if (0 < m_pVoice->GetState().BuffersQueued && m_pVoice->GetState().BuffersQueued < 2 && m_pVoice->IsRunnning()) {
		if (m_currentLoopCount == m_loopCount + 1) {
			return S_OK;
		}

		if (m_pWaveData[m_index].empty()) {
			m_pWaveData[m_index].resize(m_division_size);
		}
		if (!m_ploader) {
			return (E_FAIL);
		}
		DWORD size;
		m_ploader->Read(m_pWaveData[m_index].data(), m_currentPos, m_division_size, &size);
		m_currentPos += size;
		auto buffer = m_pVoice->GetBuffer();

		if (m_currentPos >= m_ploader->GetSize()) {
			m_currentPos = GetLoopPos();
			if (m_currentLoopCount == m_loopCount) {
				buffer.Flags = XAUDIO2_END_OF_STREAM;
			}
			m_currentLoopCount++;
		}

		buffer.pAudioData = m_pWaveData[m_index].data();
		buffer.AudioBytes = size;
		m_pVoice->SubmitBuffer(buffer);

		m_index = (m_index + 1) % 2;
	}
	return S_OK;
}

float CSoundStreaming::GetCurrentPos() const {
	if (!m_ploader || !m_pVoice) {
		return 0.0f;
	}
	auto pos = m_pVoice->GetCurrentSample() * m_ploader->GetFormat().wBitsPerSample / 4;
	if (pos <= m_ploader->GetSize()) {
		return (float)pos / (float)m_ploader->GetSize();
	}
	return (float)((pos - m_ploader->GetSize()) % (m_ploader->GetSize() - GetLoopPos()) + GetLoopPos()) / (float)m_ploader->GetSize();
}