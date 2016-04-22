#include "JDepthStencil.h"
#include "JApplication.h"
#include "JUtility.h"
//#include "JTexture.hpp"

using namespace Javelin;

CDepthStencil::CDepthStencil() noexcept :
m_pDepthStencilView(nullptr) {
}

CDepthStencil::~CDepthStencil() noexcept {
	Cleanup();
}

CDepthStencil::CDepthStencil(const CDepthStencil& DepthStencil) {
	operator=(DepthStencil);
}

CDepthStencil& CDepthStencil::operator=(const CDepthStencil& DepthStencil) {
	if (this == &DepthStencil) {
		return *this;
	}
	Cleanup();
	CTexture2D::operator=(DepthStencil);
	CreateDSV();
	return *this;
}

CDepthStencil::CDepthStencil(CDepthStencil&& DepthStencil) {
	operator=(std::move(DepthStencil));
}

CDepthStencil& CDepthStencil::operator=(CDepthStencil&& DepthStencil) {
	if (this == &DepthStencil) {
		return *this;
	}
	Cleanup();
	CTexture2D::operator=(DepthStencil);
	m_pDepthStencilView = DepthStencil.m_pDepthStencilView;
	return *this;
}

void CDepthStencil::Initialize(UINT width, UINT height) {
	Cleanup();

	D3D11_TEXTURE2D_DESC desc{};
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Width = width;
	desc.Height = height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	CTexture2D::Initialize(desc, &srvDesc, nullptr);
	CreateDSV();
}

void CDepthStencil::CreateDSV() {
	if (Application::GetDevice()) {
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Flags = 0;
		dsvDesc.Texture2D.MipSlice = 0;
		if (FAILED(Application::GetDevice()->CreateDepthStencilView(GetTexture(),
			&dsvDesc, &m_pDepthStencilView))) {
			Application::WriteLog("バッファの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CDepthStencil::Cleanup() noexcept {
	SAFE_RELEASE(m_pDepthStencilView);
	CTexture2D::Cleanup();
}
