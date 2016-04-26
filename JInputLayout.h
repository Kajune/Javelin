#pragma once

#include "JResource.h"
#include "JShader.h"

namespace Javelin {

	class CInputLayout : public CResource {
	protected:
		ID3D11InputLayout*	m_pInputLayout;
	public:
		CInputLayout() noexcept;
		virtual ~CInputLayout() noexcept;
		CInputLayout(const CInputLayout&) = delete;
		CInputLayout& operator=(const CInputLayout&) = delete;
		CInputLayout(CInputLayout&&);
		CInputLayout& operator=(CInputLayout&&);

		void Initialize(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
			UINT numElements, const CVertexShader& vertexShader);
		void Cleanup() noexcept;

		ID3D11InputLayout* GetInputLayout() const noexcept {
			return m_pInputLayout;
		}
	};

}

