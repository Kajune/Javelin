#pragma once

#include "JResource.h"

namespace Javelin {

	class CRasterizerState : public CResource {
	protected:
		ID3D11RasterizerState*	m_pRasterizerState;
		D3D11_RASTERIZER_DESC	m_rasterizerDesc;
	public:
		CRasterizerState() noexcept;
		virtual ~CRasterizerState() noexcept;
		CRasterizerState(const CRasterizerState&);
		CRasterizerState& operator=(const CRasterizerState&);
		CRasterizerState(CRasterizerState&&);
		CRasterizerState& operator=(CRasterizerState&&);

		void Initialize(const D3D11_RASTERIZER_DESC& rsDesc);
		void ApplyChange();
		void Cleanup() noexcept;

		D3D11_RASTERIZER_DESC& GetRasterizerDesc() noexcept {
			return m_rasterizerDesc;
		}
		ID3D11RasterizerState* GetRasterizerState() const noexcept {
			return m_pRasterizerState;
		}
	};

}

