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

CRenderTarget::CRenderTarget(const CRenderTarget& renderTarget) {
	operator=(renderTarget);
}

CRenderTarget& CRenderTarget::operator=(const CRenderTarget& renderTarget) {
	if (this == &renderTarget) {
		return *this;
	}
	Cleanup();
	CTexture2D::operator=(renderTarget);
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
	return *this;
}

void CRenderTarget::Initialize(UINT width, UINT height, DXGI_FORMAT format) {
	Cleanup();

	D3D11_TEXTURE2D_DESC desc{};
	desc.ArraySize = 1;
	desc.Format = format;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.Width = width;
	desc.Height = height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.CPUAccessFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	CTexture2D::Initialize(desc, nullptr, nullptr);
	CreateRTV();
}

void CRenderTarget::CreateRTV() {
	if (Application::GetDevice()) {
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = m_desc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;
		if (FAILED(Application::GetDevice()->CreateRenderTargetView(GetTexture(),
			&rtvDesc, &m_pRenderTargetView))) {
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
