#pragma once

#include "JResource.h"

namespace Javelin {

	class CDepthStencilState : public CResource {
	protected:
		ID3D11DepthStencilState*	m_pDepthSteincilState;
		D3D11_DEPTH_STENCIL_DESC	m_DepthSteincilDesc;
	public:
		CDepthStencilState() noexcept;
		virtual ~CDepthStencilState() noexcept;
		CDepthStencilState(const CDepthStencilState&);
		CDepthStencilState& operator=(const CDepthStencilState&);
		CDepthStencilState(CDepthStencilState&&);
		CDepthStencilState& operator=(CDepthStencilState&&);

		void Initialize(const D3D11_DEPTH_STENCIL_DESC& rsDesc);
		void Cleanup() noexcept;

		ID3D11DepthStencilState* GetDepthStencilState() const noexcept {
			return m_pDepthSteincilState;
		}
	};

}

