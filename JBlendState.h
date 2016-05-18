#pragma once

#include "JResource.h"
#include "JUtility.h"

namespace Javelin {

	class CBlendState : public CResource {
	protected:
		ID3D11BlendState*	m_pBlendState;
		D3D11_BLEND_DESC	m_BlendDesc;
	public:
		CBlendState() noexcept;
		virtual ~CBlendState() noexcept;

		void Initialize();
		void Cleanup() noexcept;

		void SetBlendDesc(const D3D11_BLEND_DESC& blendDesc) noexcept;
		void SetEnableBlend(bool value, UINT renderTarget = 0) noexcept;
		void SetAlignmentBlend(UINT renderTarget = 0) noexcept;
		void SetAddBlend(UINT renderTarget = 0) noexcept;
		void SetSubtractBlend(UINT renderTarget = 0) noexcept;
		void SetMultipleBlend(UINT renderTarget = 0) noexcept;
		void SetFactorBlend(UINT renderTarget = 0) noexcept;
		void ApplyChange();

		D3D11_BLEND_DESC& GetBlendDesc() noexcept {
			return m_BlendDesc;
		}

		UINT GetBlendRenderTargetNum() const noexcept {
			return array_length(m_BlendDesc.RenderTarget);
		}

		ID3D11BlendState* GetBlendState() const noexcept {
			return m_pBlendState;
		}
		operator bool() const override {
			return m_pBlendState != nullptr;
		}
	};

}

