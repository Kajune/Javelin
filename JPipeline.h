#pragma once
#include "JGlobal.h"
#include <memory>
#include <vector>
#include "JVertexAndIndexBuffer.h"

namespace Javelin {

	class CInputLayout;

	class CVertexShader;
	class CHullShader;
	class CDomainShader;
	class CGeometryShader;
	class CComputeShader;
	class CPixelShader;

	class CRasterizerState;
	class CViewport;

	class CRenderTarget;
	class CDepthStencil;
	class CDepthStencilState;
	class CBlendState;

	class CPipeline final {
		std::shared_ptr<ID3D11DeviceContext> m_pDeviceContext;
	public:
		CPipeline() noexcept;
		~CPipeline() noexcept = default;

		void Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext);

		//IA
		template <typename VertexType>
		void SetVertexBuffer(UINT startSlot, UINT bufferNum, const CVertexBuffer<VertexType>* ppBuffer[], 
			const UINT* pStrides, const UINT* pOffsets) const;
		void SetIndexBuffer(const CIndexBuffer* pIndexBuffer, UINT offset) const;
		void SetInputLayout(const CInputLayout* pInputLayout) const;
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) const;

		//VS
		void SetVertexShader(const CVertexShader* pShader) const;

		//HS
		void SetHullShader(const CHullShader* pShader) const;

		//DS
		void SetDomainShader(const CDomainShader* pShader) const;

		//GS
		void SetGeometryShader(const CGeometryShader* pShader) const;

		//CS
		void SetComputeShader(const CComputeShader* pShader) const;

		//SO

		//RS
		void SetRasterizerState(const CRasterizerState* pRasterizerState) const;
		void SetViewports(UINT numViewports, const CViewport* ppViewports[]) const;

		//PS
		void SetPixelShader(const CPixelShader* pShader) const;

		//OM
		void SetRenderTarget(UINT numRenderTargets, const CRenderTarget* ppRenderTarget[], 
			const CDepthStencil* depthStencil) const;
		void SetRenderTarget(UINT numRenderTargets, ID3D11RenderTargetView* ppRenderTarget[],
			const CDepthStencil* depthStencil) const;
		void SetDepthStencilState(const CDepthStencilState* depthStencilState, UINT stencilRef) const;
		void SetBlendState(const CBlendState* pBlendState, 
			const COLOR& blendFactor = COLOR(0.0f, 0.0f, 0.0f, 0.0f), UINT sampleMask = 0xffffff) const;

		//描画
		void Draw(UINT numVertex, UINT startVertex = 0) const;
		void DrawIndexed(UINT numIndex, UINT startIndex = 0, int baseVertex = 0) const;
		void DrawInstanced(UINT numVertexPerInstance, UINT numInstance,
						UINT startVertex = 0, UINT startInstance = 0) const;
		void DrawIndexedInstanced(UINT numIndexPerInstance, UINT numInstance,
			UINT startIndex = 0, int baseVertex = 0, UINT startInstance = 0) const;
		void DrawAuto() const;
	};

	template <typename VertexType>
	void CPipeline::SetVertexBuffer(UINT startSlot, UINT bufferNum, const CVertexBuffer<VertexType>* pBuffer[],
		const UINT* pStrides, const UINT* pOffsets) const {
		if (!m_pDeviceContext) {
			Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
			return;
		}

		if (bufferNum == 1) {
			ID3D11Buffer* buffer[] = { pBuffer[0]->GetBuffer() };
			m_pDeviceContext->IASetVertexBuffers(startSlot, 1, buffer, pStrides, pOffsets);
		} else {
			std::vector<ID3D11Buffer*> buffer;
			buffer.resize(bufferNum);
			for (UINT i = 0; i < bufferNum; i++) {
				buffer.at(i) = pBuffer[i]->GetBuffer();
			}
			m_pDeviceContext->IASetVertexBuffers(startSlot, bufferNum, buffer.data(), pStrides, pOffsets);
		}
	}

}