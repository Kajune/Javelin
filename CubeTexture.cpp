#include "JCubeTexture.h"

using namespace Javelin;

CCubeTexture::~CCubeTexture() noexcept {
	Cleanup();
}

void CCubeTexture::Initialize(UINT width, UINT height, DXGI_FORMAT format, UINT mipLevels) {
	D3D11_TEXTURE2D_DESC rtDesc{};
	rtDesc.Width = width;
	rtDesc.Height = height;
	rtDesc.Format = format;
	rtDesc.MipLevels = mipLevels;
	rtDesc.ArraySize = 6;
	rtDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	rtDesc.CPUAccessFlags = 0;
	rtDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;
	rtDesc.Usage = D3D11_USAGE_DEFAULT;
	rtDesc.SampleDesc.Count = 1;
	rtDesc.SampleDesc.Quality = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = rtDesc.Format;
	srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = mipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = rtDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	rtvDesc.Texture2DArray.ArraySize = 6;
	rtvDesc.Texture2DArray.MipSlice = 0;

	m_renderTarget.Initialize(rtDesc, &srvDesc, &rtvDesc);

	D3D11_TEXTURE2D_DESC dsDesc = rtDesc;
	dsDesc.MipLevels = 1;
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = dsDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.FirstArraySlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 6;
	dsvDesc.Texture2DArray.MipSlice = 0;

	m_depthStencil.Initialize(dsDesc, nullptr, &dsvDesc, false);
}

void CCubeTexture::Cleanup() noexcept {
	m_renderTarget.Cleanup();
	m_depthStencil.Cleanup();
}

