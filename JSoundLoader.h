#pragma once

#include "XAudio2.h"
#include "JGlobal.h"
#include "JResource.h"
#include <vector>

namespace Javelin {

	class CSoundLoader : CResource {
	protected:
		std::string m_filename;

		WAVEFORMATEX	m_pwfx;

		DWORD			m_size;
		DWORD			m_loopPos;
		bool			m_available;

	public:
		CSoundLoader();

		void Initialize(std::string filename, DWORD loopPos);
		virtual void Cleanup() noexcept = 0;

		DWORD GetSize() const {
			return m_size;
		}
		DWORD GetLoopPos() const {
			return m_loopPos;
		}
		const WAVEFORMATEX& GetFormat() const noexcept{
			return m_pwfx;
		}

		virtual void Load() = 0;
		virtual void Read(BYTE* pBuffer, DWORD begin, DWORD sizeToRead, DWORD* pSizeRead) = 0;

		operator bool() const override {
			return m_available;
		}
	};

}
