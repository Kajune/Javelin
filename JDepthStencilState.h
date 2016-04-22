#pragma once

#include "JResource.h"

namespace Javelin {

	class CDepthSteincilState : public CResource {
	protected:
		ID3D11DepthStencilState*	m_pDepthSteincilState;
		D3D11_DEPTH_STENCIL_DESC	m_DepthSteincilDesc;
	public:
		CDepthSteincilState() noexcept;
		virtual ~CDepthSteincilState() noexcept;
		CDepthSteincilState(const CDepthSteincilState&);
		CDepthSteincilState& operator=(const CDepthSteincilState&);
		CDepthSteincilState(CDepthSteincilState&&);
		CDepthSteincilState& operator=(CDepthSteincilState&&);

		void Initialize(const D3D11_DEPTH_STENCIL_DESC& rsDesc);
		void Cleanup() noexcept;

		ID3D11DepthStencilState* GetDepthSteincilState() const noexcept {
			return m_pDepthSteincilState;
		}
	};

}

