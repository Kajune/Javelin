#pragma once

#include "JResource.h"

namespace Javelin {

	class CSamplerState : public CResource {
	protected:
		ID3D11SamplerState*	m_pSamplerState;
		D3D11_SAMPLER_DESC	m_desc;
	public:
		CSamplerState() noexcept;
		virtual ~CSamplerState() noexcept;

		void Initialize(const D3D11_SAMPLER_DESC& desc);
		void Initialize(D3D11_TEXTURE_ADDRESS_MODE textureAddress, 
			D3D11_FILTER filter = D3D11_FILTER_ANISOTROPIC,
			const COLOR& borderColor = COLOR() , UINT maxAnisotropy = 16);
		void Cleanup() noexcept;

		ID3D11SamplerState* GetSamplerState() const noexcept {
			return m_pSamplerState;
		}
	};

}

