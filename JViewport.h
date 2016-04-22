#pragma once

#include "JResource.h"

namespace Javelin {

	class CViewport : public CResource {
	protected:
		D3D11_VIEWPORT	m_viewport;
	public:
		CViewport() noexcept;
		virtual ~CViewport() noexcept;
		CViewport(const CViewport&) noexcept;
		CViewport& operator=(const CViewport&) noexcept;
		CViewport(CViewport&&) noexcept;
		CViewport& operator=(CViewport&&) noexcept;

		void Initialize(UINT width, UINT height) noexcept;
		void Initialize(const D3D11_VIEWPORT& viewport) noexcept;

		const D3D11_VIEWPORT* GetViewport() const noexcept {
			return &m_viewport;
		}
	};

}

