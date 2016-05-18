#pragma once

#include "JRenderTarget.h"
#include "JDepthStencil.h"

namespace Javelin {

	class CCubeTexture : public CShaderResourceView {
		CRenderTarget m_renderTarget;
		CDepthStencil m_depthStencil;
	public:
		virtual ~CCubeTexture() noexcept;

		void Initialize(UINT width, UINT height, DXGI_FORMAT format, UINT mipLevels = 1);
		void Cleanup() noexcept;

		ID3D11ShaderResourceView* GetShaderResourceView() const noexcept override {
			return m_renderTarget.GetShaderResourceView();
		}
		ID3D11RenderTargetView* GetRenderTargetView() const noexcept {
			return m_renderTarget.GetRenderTargetView();
		}
		ID3D11DepthStencilView* GetDepthStencilView() const noexcept {
			return m_depthStencil.GetDepthStencilView();
		}
		operator bool() const override {
			return m_renderTarget && m_depthStencil;
		}
	};

}

