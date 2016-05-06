#include "JRenderTarget.h"
#include "JApplication.h"
#include "JUtility.h"
#include "JTexture.hpp"

using namespace Javelin;

CRenderTarget::CRenderTarget() noexcept :
m_pRenderTargetView(nullptr){
}

CRenderTarget::~CRenderTarget() noexcept {
	Cleanup();
}

/*
CRenderTarget::CRenderTarget(const CRenderTarget& renderTarget) {
	operator=(renderTarget);
}

CRenderTarget& CRenderTarget::operator=(const CRenderTarget& renderTarget) {
	if (this == &renderTarget) {
		return *this;
	}
	Cleanup();
	CTexture2D::operator=(renderTarget);
	m_rtvDesc = renderTarget.m_rtvDesc;
	CreateRTV();
	return *this;
}

CRenderTarget::CRenderTarget(CRenderTarget&& renderTarget) {
	operator=(std::move(renderTarget));
}

CRenderTarget& CRenderTarget::operator=(CRenderTarget&& renderTarget) {
	if (this == &renderTarget) {
		return *this;
	}
	Cleanup();
	CTexture2D::operator=(renderTarget);
	m_pRenderTargetView = renderTarget.m_pRenderTargetView;
	m_rtvDesc = renderTarget.m_rtvDesc;
	return *this;
}*/

void CRenderTarget::Initialize(UINT width, UINT height, DXGI_FORMAT format, UINT mipLevels) {
	D3D11_TEXTURE2D_DESC desc{};
	desc.ArraySize = 1;
	desc.Format = format;
	desc.MipLevels = mipLevels;
	desc.MiscFlags = mipLevels > 0 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
	desc.Width = width;
	desc.Height = height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	Initialize(desc, nullptr, nullptr);
}

void CRenderTarget::Initialize(const DescType& desc, 
	const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
	const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc) {
	Cleanup();
	CTexture2D::Initialize(desc, srvDesc, nullptr, true);
	if (rtvDesc) {
		m_rtvDesc = *rtvDesc;
	} else {
		m_rtvDesc.Format = m_desc.Format;
		m_rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		m_rtvDesc.Texture2D.MipSlice = 0;
	}
	CreateRTV();
}

void CRenderTarget::CreateRTV() {
	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateRenderTargetView(GetTexture(),
			&m_rtvDesc, &m_pRenderTargetView))) {
			Application::WriteLog("バッファの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CRenderTarget::Cleanup() noexcept {
	SAFE_RELEASE(m_pRenderTargetView);
	CTexture2D::Cleanup();
}
