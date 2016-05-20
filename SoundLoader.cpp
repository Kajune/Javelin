#include "JSoundLoader.h"

using namespace Javelin;

CSoundLoader::CSoundLoader() :
m_available(false) {
}

void CSoundLoader::Initialize(std::string filename, DWORD loopPos) {
	m_filename = filename;
	m_loopPos = loopPos;

	Cleanup();
	Load();
}
