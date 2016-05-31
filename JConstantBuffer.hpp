#include "JConstantBuffer.h"

namespace Javelin {

	template <typename BufferType>
	void CConstantBuffer<BufferType>::Initialize() {
		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(BufferType);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		CBuffer::Initialize(desc, nullptr);
	}

	template <typename BufferType>
	void CConstantBuffer<BufferType>::UpdateBufferValue(const BufferType& buffer,
		std::shared_ptr<ID3D11DeviceContext> pDeviceContext) {
		if (!pDeviceContext) {
			Application::WriteLog("CDynamicVertexAndIndexBuffer::WriteBuffer デバイスコンテキストがセットされていません");
			return;
		}
		D3D11_MAPPED_SUBRESOURCE res;
		if (FAILED(pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))) {
			Application::WriteLog("CDynamicVertexAndIndexBuffer::WriteBuffer 定数バッファのロックに失敗しました");
			throw - 1;
		}

		memcpy_s(res.pData, sizeof(BufferType), &buffer, sizeof(BufferType));

		pDeviceContext->Unmap(m_pBuffer, 0);
	}
}
