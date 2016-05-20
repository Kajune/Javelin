#pragma once

#include "XAudio2.h"
#include <list>
#include <memory>
#include <vector>

namespace Javelin {

	class CSoundLoader;
	class CVoice;
	class SoundPlayer;

	enum SOUNDFORMAT {
		SF_DEFAULT = 0,
		SF_WAVE,
		SF_DUMMY,
	};

	class CSound {
		friend class SoundPlayer;
		friend class Application;

		static std::list<CSound*> soundList;
	protected:
		std::unique_ptr<CSoundLoader>	m_ploader;
		mutable std::vector<BYTE>		m_pWaveData[2];
		mutable float					m_volume;
		mutable float					m_pitch;

		mutable std::list<std::shared_ptr<CVoice>> m_voiceList;

		HRESULT play_sub(IXAudio2* audio2, const XAUDIO2_BUFFER* buffer, DWORD size,
			DWORD* sizeSubmit, std::shared_ptr<CVoice>* ppVoice) const;
		virtual HRESULT play(IXAudio2* audio2, const XAUDIO2_BUFFER* buffer, std::shared_ptr<CVoice>* ppVoice) const;
		virtual HRESULT pause() const;
		virtual HRESULT resume() const;
		virtual HRESULT stop() const;
		virtual void MainLoop();

		static CSoundLoader* createLoader(std::string extension);
		static void MainLoopAll();
	public:
		CSound();
		virtual ~CSound();

		//filenameの拡張子と音声ファイルのフォーマットが異なる場合、formatに正しいフォーマットを指定する
		void Initialize(std::string filename, DWORD loopPos = 0, SOUNDFORMAT format = SF_DEFAULT);
		DWORD GetLoopPos() const;
		void SetVolume(float volume) const;
		operator bool() const noexcept;
		float GetVolume() const noexcept {
			return m_volume;
		}

		void SetFrequencyRatio(float ratio) const;
		float inline GetFrequencyRatio() const noexcept {
			return m_pitch;
		}
	};

	class CSoundStreaming : public CSound {
		UINT							m_division_size;
		mutable int						m_index;
		mutable DWORD					m_currentPos;
		mutable std::shared_ptr<CVoice>	m_pVoice;
		mutable int						m_loopCount;
		mutable int						m_currentLoopCount;

		HRESULT play(IXAudio2* audio2, const XAUDIO2_BUFFER* buffer, std::shared_ptr<CVoice>* ppVoice) const override;
		HRESULT pause() const;
		HRESULT resume() const;
		HRESULT stop() const override;
		HRESULT Polling();
		virtual void MainLoop() override;
	public:
		CSoundStreaming();
		~CSoundStreaming();

		//divide_numで分割数を指定する。4の倍数を指定する必要がある。
		//filenameの拡張子と音声ファイルのフォーマットが異なる場合、formatに正しいフォーマットを指定する
		void Initialize(std::string filename, UINT division_size,
			DWORD loopPos = 0, SOUNDFORMAT format = SF_DEFAULT);
		float GetCurrentPos() const;
	};

}
