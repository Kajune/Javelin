#pragma once

#include "JResource.h"

namespace Javelin {

	class CBuffer : public CResource {
	protected:
		ID3D11Buffer*		m_pBuffer;
		D3D11_BUFFER_DESC	m_desc;
	public:
		CBuffer() noexcept;
		virtual ~CBuffer() noexcept;

		CBuffer(const CBuffer&);
		CBuffer& operator=(const CBuffer&);
		CBuffer(CBuffer&&);
		CBuffer& operator=(CBuffer&&);


		void Initialize(const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* subResource);
		void Cleanup() noexcept;

		ID3D11Buffer* GetBuffer() const noexcept {
			return m_pBuffer;
		}
	};

}
