#pragma once

#include "XAudio2.h"

namespace Javelin {

	class CSound;
	class CSoundStreaming;

	class CVoice {
		friend class CSound;
		friend class CSoundStreaming;
		
		IXAudio2SourceVoice*	m_pSourceVoice;
		bool					m_isRunning;
		bool					m_isPaused;
		XAUDIO2_BUFFER			m_buffer;
		DWORD					m_loopPos;
		XAUDIO2_VOICE_STATE		m_state;
		const int				kInputChannels;

		HRESULT SubmitBuffer(const XAUDIO2_BUFFER buffer);
		HRESULT Polling();

		DWORD inline GetCurrentSample() const {
			return (int)m_state.SamplesPlayed;
		}
		const inline XAUDIO2_VOICE_STATE& GetState() const {
			return m_state;
		}
		const inline XAUDIO2_BUFFER& GetBuffer() const {
			return m_buffer;
		}
	public:
		CVoice(IXAudio2SourceVoice* pSourceVoice,
			const XAUDIO2_BUFFER& buffer, DWORD loopPos, int inputChannels);
		~CVoice();

		HRESULT Pause();
		HRESULT Resume();
		HRESULT Stop();
		HRESULT SetVolume(float volume) const;
		float GetVolume() const;
		HRESULT SetFrequencyRatio(float ratio) const;
		float GetFrequencyRatio() const;
		HRESULT SetOutputMatrix(int inputChannels, int outputChannels, const float *pLevelMatrix) const;

		inline bool IsRunnning() const {
			return m_isRunning;
		}
		inline bool IsPaused() const {
			return m_isPaused;
		}
	};

}
