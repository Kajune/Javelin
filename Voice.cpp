#include "JVoice.h"

using namespace Javelin;

//
// CVoice
//

CVoice::CVoice(IXAudio2SourceVoice* pSourceVoice, const XAUDIO2_BUFFER& buffer, DWORD loopPos, int inputChannels) :
m_pSourceVoice(pSourceVoice),
m_buffer(buffer),
m_isRunning(true),
m_isPaused(false),
m_loopPos(loopPos),
kInputChannels(inputChannels){
}

CVoice::~CVoice() {
}

HRESULT CVoice::Polling() {
	if (!m_pSourceVoice) {
		return S_FALSE;
	}
	m_pSourceVoice->GetState(&m_state);
	if (m_state.BuffersQueued == 0) {
		m_pSourceVoice = nullptr;
		m_isRunning = false;
	}
	return S_OK;
}

HRESULT CVoice::Pause() {
	if (!m_pSourceVoice || !IsRunnning()) {
		return S_FALSE;
	}
	m_isRunning = false;
	m_isPaused = true;
	return m_pSourceVoice->Stop();
}

HRESULT CVoice::Resume() {
	if (!m_pSourceVoice || !IsPaused()) {
		return S_FALSE;
	}
	m_isRunning = true;
	m_isPaused = false;
	return m_pSourceVoice->Start();
}

HRESULT CVoice::Stop() {
	if (!m_pSourceVoice || !IsRunnning()) {
		return S_FALSE;
	}
	m_isRunning = false;
	m_isPaused = false;
	return m_pSourceVoice->Stop();
}

HRESULT CVoice::SubmitBuffer(const XAUDIO2_BUFFER buffer) {
	if (!m_pSourceVoice) {
		return S_FALSE;
	}
	return m_pSourceVoice->SubmitSourceBuffer(&buffer);
}

HRESULT CVoice::SetVolume(float volume) const {
	if (!m_pSourceVoice) {
		return S_FALSE;
	}
	return m_pSourceVoice->SetVolume(volume);
}

float CVoice::GetVolume() const {
	if (!m_pSourceVoice) {
		return S_FALSE;
	}
	float volume;
	m_pSourceVoice->GetVolume(&volume);
	return volume;
}

HRESULT CVoice::SetFrequencyRatio(float ratio) const {
	if (!m_pSourceVoice) {
		return S_FALSE;
	}
	return m_pSourceVoice->SetFrequencyRatio(ratio);
}

float CVoice::GetFrequencyRatio() const {
	if (!m_pSourceVoice) {
		return 1.0f;
	}
	float ratio;
	m_pSourceVoice->GetFrequencyRatio(&ratio);
	return ratio;
}

HRESULT CVoice::SetOutputMatrix(int inputChannels, int outputChannels, const float *pLevelMatrix) const {
	if (!m_pSourceVoice) {
		return S_FALSE;
	}
	return m_pSourceVoice->SetOutputMatrix(nullptr, kInputChannels, outputChannels, pLevelMatrix);
}