#pragma once

#include "JTexture.h"

namespace Javelin {

	class CDepthStencil : public CTexture2D {
	protected:
		ID3D11DepthStencilView*	m_pDepthStencilView;

		void CreateDSV();
	public:
		CDepthStencil() noexcept;
		virtual ~CDepthStencil() noexcept;
		CDepthStencil(const CDepthStencil&);
		CDepthStencil& operator=(const CDepthStencil&);
		CDepthStencil(CDepthStencil&&);
		CDepthStencil& operator=(CDepthStencil&&);

		void Initialize(UINT width, UINT height);
		void Cleanup() noexcept;

		ID3D11DepthStencilView* GetDepthStencilView() const noexcept {
			return m_pDepthStencilView;
		}
	};

}

