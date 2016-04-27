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

		ID3D11Buffer* buffer[] = { pBuffer->GetBuffer() };
		UINT strides[] = { stride == 0 ? sizeof(VertexType) : stride };
		UINT offsets[] = { offset };
		m_pDeviceContext->IASetVertexBuffers(slot, 1, buffer, strides, offsets);
	}

	template<typename BufferType>
	void CPipeline::SetConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer,
		std::function<void(std::shared_ptr<ID3D11DeviceContext>, UINT, UINT, ID3D11Buffer**)> func) const {
		if (!m_pDeviceContext) {
			Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
			return;
		}

		ID3D11Buffer* buffer[] = { pBuffer->GetBuffer() };
		func(m_pDeviceContext, slot, 1, buffer);
	}

	template<typename BufferType>
	void CPipeline::SetVertexShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(slot, pBuffer, &ID3D11DeviceContext::VSSetConstantBuffers);
	}

	template<typename BufferType>
	void CPipeline::SetHullShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(slot, pBuffer, &ID3D11DeviceContext::HSSetConstantBuffers);
	}

	template<typename BufferType>
	void CPipeline::SetDomainShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(slot, pBuffer, &ID3D11DeviceContext::DSSetConstantBuffers);
	}

	template<typename BufferType>
	void CPipeline::SetGeometryShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(slot, pBuffer, &ID3D11DeviceContext::GSSetConstantBuffers);
	}

	template<typename BufferType>
	void CPipeline::SetComputeShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(slot, pBuffer, &ID3D11DeviceContext::CSSetConstantBuffers);
	}

	template<typename BufferType>
	void CPipeline::SetPixelShaderConstantBuffer(UINT slot,
		const CConstantBuffer<BufferType>* pBuffer) const {
		SetConstantBuffer(slot, pBuffer, &ID3D11DeviceContext::PSSetConstantBuffers);
	}

}