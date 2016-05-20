#include "JWaveLoader.h"
#include "JApplication.h"

using namespace Javelin;

CWaveLoader::CWaveLoader() :
m_hmmio(nullptr){
}

CWaveLoader::~CWaveLoader() {
	Cleanup();
}

void CWaveLoader::Load() {
	if (m_filename.empty()) {
		Application::WriteLog("指定されたファイル名が不正です");
	}

	m_hmmio = mmioOpen((LPSTR)m_filename.c_str(), nullptr, MMIO_ALLOCBUF | MMIO_READ);
	if (!m_hmmio) {
		Application::WriteLog("ファイルのオープンに失敗しました：" + m_filename);
	}

	if (FAILED(ReadMMIO()) || FAILED(ResetMMIO())) {
		mmioClose(m_hmmio, 0);
		Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
		throw - 1;
	}

	m_size = m_ck.cksize;
	m_available = true;
}

void CWaveLoader::Cleanup() noexcept {
	if (m_hmmio) {
		mmioClose(m_hmmio, 0);
		m_hmmio = nullptr;
	}
	m_available = false;
}

HRESULT CWaveLoader::ReadMMIO() {
	if (mmioDescend(m_hmmio, &m_ckRiff, nullptr, 0) != MMSYSERR_NOERROR) {
		return (E_FAIL);
	}

	if (m_ckRiff.ckid != FOURCC_RIFF ||
		m_ckRiff.fccType != mmioFOURCC('W', 'A', 'V', 'E')) {
		return (E_FAIL);
	}

	MMCKINFO ckIn;
	SecureZeroMemory(&ckIn, sizeof(ckIn));
	ckIn.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if (mmioDescend(m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK) != MMSYSERR_NOERROR) {
		return (E_FAIL);
	}

	if (ckIn.cksize < (LONG)sizeof(PCMWAVEFORMAT)) {
		return (E_FAIL);
	}

	PCMWAVEFORMAT pcmWaveFormat;
	if (mmioRead(m_hmmio, (HPSTR)&pcmWaveFormat, sizeof(pcmWaveFormat)) != sizeof(pcmWaveFormat)) {
		return (E_FAIL);
	}

	if (pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM) {
		memcpy_s(&m_pwfx, sizeof(WAVEFORMATEX), &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx.cbSize = 0;
	} else {
		WORD cbExtraBytes = 0L;
		if (mmioRead(m_hmmio, (CHAR*)cbExtraBytes, sizeof(WORD)) != sizeof(WORD)) {
			return (E_FAIL);
		}
		memcpy_s(&m_pwfx, sizeof(WAVEFORMATEX), &pcmWaveFormat, sizeof(pcmWaveFormat));
		m_pwfx.cbSize = cbExtraBytes;
		if (mmioRead(m_hmmio, (CHAR*)(((BYTE*)&(m_pwfx.cbSize)) + sizeof(WORD)), cbExtraBytes) != cbExtraBytes) {
			return (E_FAIL);
		}
	}

	if (mmioAscend(m_hmmio, &ckIn, 0) != 0) {
		return (E_FAIL);
	}

	return S_OK;
}

HRESULT CWaveLoader::ResetMMIO() {
	if (!m_hmmio) {
		return (CO_E_NOTINITIALIZED);
	}
	if (mmioSeek(m_hmmio, m_ckRiff.dwDataOffset + sizeof(FOURCC), SEEK_SET) == -1) {
		return (E_FAIL);
	}
	m_ck.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if (mmioDescend(m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK) == -1) {
		return (E_FAIL);
	}

	return S_OK;
}

void CWaveLoader::Read(BYTE* pBuffer, DWORD begin, DWORD sizeToRead, DWORD* pSizeRead){
	if (!m_hmmio) {
		return;
	}
	if (!pBuffer) {
		return;
	}

	if (pSizeRead) {
		*pSizeRead = 0;
	}

	if (FAILED(ResetMMIO())) {
		Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
		throw - 1;
	}

	MMIOINFO mmioInfoIn;
	if (mmioGetInfo(m_hmmio, &mmioInfoIn, 0) != 0) {
		Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
		throw - 1;
	}

	UINT cbDataIn = sizeToRead + begin;
	if (cbDataIn > GetSize()) {
		cbDataIn = GetSize();
	}

	for (DWORD cT = 0; cT < begin; cT++) {
		if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead) {
			if (mmioAdvance(m_hmmio, &mmioInfoIn, MMIO_READ) != 0) {
				Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
				throw - 1;
			}

			if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead) {
				Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
				throw - 1;
			}
		}
		mmioInfoIn.pchNext++;
	}

	for (DWORD cT = 0; cT < cbDataIn - begin; cT++) {
		if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead) {
			if (mmioAdvance(m_hmmio, &mmioInfoIn, MMIO_READ) != 0) {
				Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
				throw - 1;
			}

			if (mmioInfoIn.pchNext == mmioInfoIn.pchEndRead) {
				Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
				throw - 1;
			}
		}

		*((BYTE*)pBuffer + cT) = *((BYTE*)mmioInfoIn.pchNext);
		mmioInfoIn.pchNext++;
	}

	if (mmioSetInfo(m_hmmio, &mmioInfoIn, 0) != 0) {
		Application::WriteLog("ファイルの読み込みに失敗しました：" + m_filename);
		throw - 1;
	}

	if (pSizeRead) {
		*pSizeRead = cbDataIn - begin;
	}
}

