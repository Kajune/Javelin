#pragma once

#include "JTexture.h"

namespace Javelin {

	class CRenderTarget : public CTexture2D {
	protected:
		ID3D11RenderTargetView*	m_pRenderTargetView;
		
		void CreateRTV();
	public:
		CRenderTarget() noexcept;
		virtual ~CRenderTarget() noexcept;
		CRenderTarget(const CRenderTarget&);
		CRenderTarget& operator=(const CRenderTarget&);
		CRenderTarget(CRenderTarget&&);
		CRenderTarget& operator=(CRenderTarget&&);

		void Initialize(UINT width, UINT height, DXGI_FORMAT format);
		void Cleanup() noexcept;

		ID3D11RenderTargetView* GetRenderTargetView() const noexcept {
			return m_pRenderTargetView;
		}
	};

}

