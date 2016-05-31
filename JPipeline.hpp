#pragma once
#include "JPipeline.h"
#include "JVertexAndIndexBuffer.h"
#include "JConstantBuffer.h"

namespace Javelin {

	template <typename VertexType>
	void CPipeline::SetVertexBuffer(UINT slot, const CVertexBuffer<VertexType>* pBuffer,
		UINT stride, UINT offset) const {
		if (!m_pDeviceContext) {
			Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
			return;
		}
		if (m_store) {
			if (m_store->vertexBuffer.find(slot) == m_store->vertexBuffer.end()) {
				Storage::vbufferSet_t vbufferSet;
				m_pDeviceContext->IAGetVertexBuffers(slot, 1, &vbufferSet.buffer, 
					&vbufferSet.stride, &vbufferSet.offset);
				m_store->vertexBuffer.emplace(slot, vbufferSet);
				m_store->changed_vertexBuffer = true;
			}
		}

		ID3D11Buffer* buffer[] = { pBuffer->GetBuffer() };
		UINT strides[] = { stride == 0 ? sizeof(VertexType) : stride };
		UINT offsets[] = { offset };
		m_pDeviceContext->IASetVertexBuffers(slot, 1, buffer, strides, offsets);
	}

	template<typename BufferType>
	void CPipeline::SetConstantBuffer(UINT ShaderType, UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		if (!m_pDeviceContext) {
			Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
			return;
		}
		if (m_store) {
			if (m_store->ShaderSet[ShaderType].ConstantBuffer.find(slot)
				== m_store->ShaderSet[ShaderType].ConstantBuffer.end()) {
				ID3D11Buffer* buffer;
				GetConstantBufferFuncs[ShaderType](*m_pDeviceContext, slot, 1, &buffer);
				m_store->ShaderSet[ShaderType].ConstantBuffer.emplace(slot, buffer);
				m_store->ShaderSet[ShaderType].changed_ConstantBuffer = true;
			}
		}
		ID3D11Buffer* buffer[] = { pBuffer ? pBuffer->GetBuffer() : nullptr };
		SetConstantBufferFuncs[ShaderType](*m_pDeviceContext, slot, 1, buffer);
	}

	template<typename BufferType>
	void CPipeline::SetVertexShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(S_VERTEX, slot, pBuffer);
	}

	template<typename BufferType>
	void CPipeline::SetHullShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(S_HULL, slot, pBuffer);
	}

	template<typename BufferType>
	void CPipeline::SetDomainShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(S_DOMAIN, slot, pBuffer);
	}

	template<typename BufferType>
	void CPipeline::SetGeometryShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(S_GEOMETRY, slot, pBuffer);
	}

	template<typename BufferType>
	void CPipeline::SetComputeShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(S_COMPUTE, slot, pBuffer);
	}

	template<typename BufferType>
	void CPipeline::SetPixelShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(S_PIXEL, slot, pBuffer);
	}

}