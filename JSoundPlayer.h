#pragma once

#include "JGlobal.h"
#include "XAudio2.h"
#include <memory>

namespace Javelin {

	class CSound;
	class CVoice;
	class CSound3D;

	class SoundPlayer : private UnConstructable {
		friend class Application;

		static IXAudio2*					m_pXAudio2;
		static IXAudio2MasteringVoice*		m_pMasterVoice;
		static XAUDIO2_DEVICE_DETAILS		m_details;

		static void Initialize();
		static void Cleanup() noexcept;
	public:
		//XAUDIO2_BUFFERを直接指定して再生する
		static void Play(const CSound& sound, const XAUDIO2_BUFFER* buffer,
			std::shared_ptr<CVoice>* ppVoice, const CSound3D* pSound3D);
		//1回だけ普通に再生する
		static void PlayNormal(const CSound& sound,
			std::shared_ptr<CVoice>* ppVoice = nullptr, const CSound3D* pSound3D = nullptr);
		//指定した回数ループ再生する　回数に負の値を指定すると無限ループできる
		static void PlayLoop(const CSound& sound, int loopTimes = -1,
			std::shared_ptr<CVoice>* ppVoice = nullptr, const CSound3D* pSound3D = nullptr);
		//再生を一時停止する
		static void Pause(const CSound& sound);
		//一時停止した再生を再開する
		static void Resume(const CSound& sound);
		//再生を停止する
		static void Stop(const CSound& sound);
		//マスタ音量を設定する
		static void SetVolume(float volume);
		//マスタ音量を得る
		static float GetVolume();
		//デバイスの詳細を得る
		static const XAUDIO2_DEVICE_DETAILS& GetDetail() {
			return m_details;
		}
	};

}
