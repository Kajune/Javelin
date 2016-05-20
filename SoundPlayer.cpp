#include "JSoundPlayer.h"
#include "JSound.h"
#include "JSound3D.h"
#include "JUtility.h"
#include "JApplication.h"

using namespace Javelin;

IXAudio2*					SoundPlayer::m_pXAudio2 = nullptr;
IXAudio2MasteringVoice*		SoundPlayer::m_pMasterVoice = nullptr;
XAUDIO2_DEVICE_DETAILS		SoundPlayer::m_details;

void SoundPlayer::Initialize() {
#if defined(DEBUG) || defined(_DEBUG)
	DWORD flag = XAUDIO2_DEBUG_ENGINE;
#else
	DWORD flag = 0;
#endif

	if (FAILED(XAudio2Create(&m_pXAudio2, flag, XAUDIO2_DEFAULT_PROCESSOR))) {
		Application::WriteLog("XAudioインターフェースの作成に失敗しました");
		throw - 1;
	}

	UINT deviceNum;
	m_pXAudio2->GetDeviceCount(&deviceNum);
	if (deviceNum <= 0) {
		Application::WriteLog("有効なデバイスが存在しません");
		return;
	}

	if (FAILED(m_pXAudio2->GetDeviceDetails(0, &m_details))) {
		Application::WriteLog("デバイス詳細の取得に失敗しました");
		throw - 1;
	}

	if (FAILED(m_pXAudio2->CreateMasteringVoice(&m_pMasterVoice, XAUDIO2_DEFAULT_CHANNELS,
		XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, nullptr))) {
		Application::WriteLog("マスタリングボイスの作成に失敗しました");
		throw - 1;
	}
}

void SoundPlayer::Cleanup() noexcept {
	if (m_pMasterVoice) {
//		m_pMasterVoice->DestroyVoice();
	}
	SAFE_RELEASE(m_pXAudio2);
}

void SoundPlayer::Play(const CSound& sound, const XAUDIO2_BUFFER* buffer, 
						   std::shared_ptr<CVoice>* ppVoice, const CSound3D* pSound3D) {
	std::shared_ptr<CVoice> pVoice;
	if (FAILED(sound.play(m_pXAudio2, buffer, &pVoice))) {
		return;
	}
	if (ppVoice) {
		*ppVoice = pVoice;
	}
	if (pSound3D) {
		pSound3D->registerVoice(pVoice);
	}
}

void SoundPlayer::PlayNormal(const CSound& sound, 
								 std::shared_ptr<CVoice>* ppVoice, const CSound3D* pSound3D) {
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.PlayBegin	= 0;
	buffer.PlayLength	= 0;
	buffer.LoopBegin	= 0;
	buffer.LoopLength	= 0;
	buffer.LoopCount	= 0;
	buffer.pContext		= nullptr;
	buffer.Flags		= XAUDIO2_END_OF_STREAM;

	Play(sound, &buffer, ppVoice, pSound3D);
}

void SoundPlayer::PlayLoop(const CSound& sound, int loopTimes, 
							   std::shared_ptr<CVoice>* ppVoice, const CSound3D* pSound3D) {
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.PlayBegin	= 0;
	buffer.PlayLength	= 0;
	buffer.LoopBegin	= (loopTimes != 0 ? sound.GetLoopPos() : 0);
	buffer.LoopLength	= 0;
	buffer.LoopCount	= (loopTimes >= 0 ? loopTimes : XAUDIO2_LOOP_INFINITE);
	buffer.pContext		= nullptr;
	buffer.Flags		= XAUDIO2_END_OF_STREAM;

	Play(sound, &buffer, ppVoice, pSound3D);
}

void SoundPlayer::Stop(const CSound& sound) {
	sound.stop();
}

void SoundPlayer::Pause(const CSound& sound) {
	sound.pause();
}

void SoundPlayer::Resume(const CSound& sound) {
	sound.resume();
}

void SoundPlayer::SetVolume(float volume) {
	if (!m_pMasterVoice) {
		return;
	}
	volume = min(volume, 1.0f);
	volume = max(volume, -1.0f);
	m_pMasterVoice->SetVolume(volume);
}

float SoundPlayer::GetVolume() {
	if (!m_pMasterVoice) {
		return 1.0f;
	}
	float volume;
	m_pMasterVoice->GetVolume(&volume);
	return volume;
}
