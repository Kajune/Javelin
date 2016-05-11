#pragma once

#include "JVertexAndIndexBuffer.h"

namespace Javelin {
	/*
	template <typename BufferType, UINT BindFlag>
	CVertexAndIndexBuffer<BufferType, BindFlag>::CVertexAndIndexBuffer(const CVertexAndIndexBuffer& buffer) {
		operator=(buffer);
	}

	template <typename BufferType, UINT BindFlag>
	CVertexAndIndexBuffer<BufferType, BindFlag>&
		CVertexAndIndexBuffer<BufferType, BindFlag>::operator=
		(const CVertexAndIndexBuffer<BufferType, BindFlag>& buffer) {
		if (this == &buffer) {
			return *this;
		}
		CBuffer::operator=(buffer);
		m_bufferLength = buffer.m_bufferLength;
		return *this;
	}

	template <typename BufferType, UINT BindFlag>
	CVertexAndIndexBuffer<BufferType, BindFlag>::CVertexAndIndexBuffer(CVertexAndIndexBuffer&& buffer) {
		operator=(std::move(buffer));
	}

	template <typename BufferType, UINT BindFlag>
	CVertexAndIndexBuffer<BufferType, BindFlag>&
		CVertexAndIndexBuffer<BufferType, BindFlag>::operator=
		(CVertexAndIndexBuffer<BufferType, BindFlag>&& buffer) {
		if (this == &buffer) {
			return *this;
		}
		CBuffer::operator=(std::move(buffer));
		m_bufferLength = buffer.m_bufferLength;
		return *this;
	}*/

	template <typename BufferType, UINT BindFlag>
	void CVertexAndIndexBuffer<BufferType, BindFlag>::Initialize(
		UINT bufferLength, const BufferType* buffer, bool useAsStreamOutputTarget) {
		m_bufferLength = bufferLength;

		D3D11_BUFFER_DESC desc{};
		desc.Usage = buffer && !useAsStreamOutputTarget ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(BufferType) * bufferLength;
		desc.BindFlags = BindFlag | (useAsStreamOutputTarget ? D3D11_BIND_STREAM_OUTPUT : 0);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subRes{};
		subRes.pSysMem = buffer;
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		CBuffer::Initialize(desc, buffer ? &subRes : nullptr);
	}

	template <typename BufferType, UINT BindFlag>
	void CDynamicVertexAndIndexBuffer<BufferType, BindFlag>::Initialize(
		UINT bufferLength, const BufferType* buffer) {
		m_bufferLength = bufferLength;

		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(BufferType) * bufferLength;
		desc.BindFlags = BindFlag;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subRes{};
		subRes.pSysMem = buffer;
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		CBuffer::Initialize(desc, buffer ? &subRes : nullptr);
	}

	template <typename BufferType, UINT BindFlag>
	void CDynamicVertexAndIndexBuffer<BufferType, BindFlag>::WriteBuffer(UINT bufferLength,
		const BufferType* src, std::shared_ptr<ID3D11DeviceContext> pDeviceContext) {
		if (!pDeviceContext) {
			Application::WriteLog("CDynamicVertexAndIndexBuffer::WriteBuffer デバイスコンテキストがセットされていません");
			return;
		}
		D3D11_MAPPED_SUBRESOURCE res;
		if (FAILED(pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))) {
			Application::WriteLog("CDynamicVertexAndIndexBuffer::WriteBuffer 頂点バッファのロックに失敗しました");
			throw - 1;
		}

		if (bufferLength > m_bufferLength) {
			Application::WriteLog(std::string("CDynamicBufferBuffer::WriteBuffer ") +
				"書き込もうとしているバッファは実際に存在するバッファサイズより大きいため、超過分が読み捨てられます");
		}

		memcpy_s(res.pData, sizeof(BufferType) * m_bufferLength, src, sizeof(BufferType) * bufferLength);

		pDeviceContext->Unmap(m_pBuffer, 0);
	}

}
