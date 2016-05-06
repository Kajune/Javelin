#pragma once

#include "JBuffer.h"
#include <memory>

namespace Javelin {

	template <typename BufferType, UINT BindFlag>
	class CVertexAndIndexBuffer : public CBuffer {
	protected:
		UINT m_bufferLength = 0;
	public:
		CVertexAndIndexBuffer() = default;
		virtual ~CVertexAndIndexBuffer() = default;

		void Initialize(UINT bufferLength, const BufferType* buffer);

		UINT GetBufferLength() const noexcept {
			return m_bufferLength;
		}

		using m_bufferType = BufferType;
	};

	template <typename BufferType, UINT BindFlag>
	class CDynamicVertexAndIndexBuffer : public CVertexAndIndexBuffer<BufferType, BindFlag> {
	public:
		void Initialize(UINT bufferLength, const BufferType* buffer);

		void WriteBuffer(UINT vertexLength, const BufferType* src,
			std::shared_ptr<ID3D11DeviceContext> pDeviceContext);
	};

	template <typename VertexType>
	using CVertexBuffer = CVertexAndIndexBuffer<VertexType, D3D11_BIND_VERTEX_BUFFER>;
	template <typename VertexType>
	using CDynamicVertexBuffer = CDynamicVertexAndIndexBuffer<VertexType, D3D11_BIND_VERTEX_BUFFER>;

	using CIndexBuffer = CVertexAndIndexBuffer<UINT, D3D11_BIND_INDEX_BUFFER>;
	using CDynamicIndexBuffer = CDynamicVertexAndIndexBuffer<UINT, D3D11_BIND_INDEX_BUFFER>;

}
