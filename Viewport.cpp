#include "JViewport.h"

using namespace Javelin;

CViewport::CViewport() noexcept {
}

CViewport::~CViewport() noexcept {
}

CViewport::CViewport(const CViewport& viewport) noexcept {
	operator=(viewport);
}

CViewport& CViewport::operator=(const CViewport& viewport) noexcept {
	m_viewport = viewport.m_viewport;
	return *this;
}

CViewport::CViewport(CViewport&& viewport) noexcept {
	operator=(std::move(viewport));
}

CViewport& CViewport::operator=(CViewport&& viewport) noexcept {
	m_viewport = viewport.m_viewport;
	return *this;
}

void CViewport::Initialize(UINT width, UINT height) noexcept {
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.MinDepth = 0.0f;
}

void CViewport::Initialize(const D3D11_VIEWPORT& viewport) noexcept {
	m_viewport = viewport;
}
