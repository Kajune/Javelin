#pragma once

#include "JTexture.h"

namespace Javelin {

	class CRenderTarget : public CTexture2D {
	protected:
		ID3D11RenderTargetView*	m_pRenderTargetView;
		D3D11_RENDER_TARGET_VIEW_DESC m_rtvDesc;
		
		void CreateRTV();
	public:
		CRenderTarget() noexcept;
		virtual ~CRenderTarget() noexcept;

		void Initialize(UINT width, UINT height, DXGI_FORMAT format,
			UINT mipLevels = 1);
		void Initialize(const DescType& desc, 
			const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
			const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc);
		void Cleanup() noexcept;

		ID3D11RenderTargetView* GetRenderTargetView() const noexcept {
			return m_pRenderTargetView;
		}
	};

}

