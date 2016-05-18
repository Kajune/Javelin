#pragma once

#include "JTexture.h"

namespace Javelin {

	class CDepthStencil : public CTexture2D {
	protected:
		ID3D11DepthStencilView*	m_pDepthStencilView;
		D3D11_DEPTH_STENCIL_VIEW_DESC m_dsvDesc;

		void CreateDSV();
	public:
		CDepthStencil() noexcept;
		virtual ~CDepthStencil() noexcept;

		void Initialize(UINT width, UINT height);
		void Initialize(const DescType& desc,
			const D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc,
			const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc,
			bool createSRV);
		void Cleanup() noexcept;

		ID3D11DepthStencilView* GetDepthStencilView() const noexcept {
			return m_pDepthStencilView;
		}
		operator bool() const override {
			return m_pDepthStencilView != nullptr && CTexture2D::operator bool();
		}
	};

}

