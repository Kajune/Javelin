#pragma once

#include "JSoundLoader.h"

namespace Javelin {

	class CWaveLoader final : public CSoundLoader {
		HMMIO			m_hmmio;
		MMCKINFO		m_ckRiff;
		MMCKINFO		m_ck;

		HRESULT ReadMMIO();
		HRESULT ResetMMIO();
	public:
		CWaveLoader();
		~CWaveLoader();

		void Cleanup() noexcept override;

		void Load() override;
		void Read(BYTE* pBuffer, DWORD begin, DWORD sizeToRead, DWORD* pSizeRead) override;
	};

}

