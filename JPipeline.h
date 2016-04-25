#pragma once
#include "JGlobal.h"
#include <memory>
#include <vector>
#include "JVertexAndIndexBuffer.h"

namespace Javelin {

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

	class CPipeline final {
		std::shared_ptr<ID3D11DeviceContext> m_pDeviceContext;
	public:
		CPipeline() noexcept;
		~CPipeline() noexcept = default;

		void Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext);

		//IA
		template <typename VertexType>
		void SetVertexBuffer(UINT startSlot, UINT bufferNum, const CVertexBuffer<VertexType>* pBuffer[], 
			const UINT* pStrides, const UINT* pOffsets) const;
		void SetIndexBuffer(const CIndexBuffer& indexBuffer, UINT offset) const;

		//VS
		void SetVertexShader(const CVertexShader& shader);

		//HS

		//DS

		//GS

		//CS

		//SO

		//RS
		void SetRasterizerState(const CRasterizerState* rasterizerState) const;
		void SetViewports(UINT numViewports, const CViewport* viewports[]) const;

		//PS

		//OM
		void SetRenderTarget(UINT numRenderTargets, const CRenderTarget* renderTarget[], 
			const CDepthStencil* depthStencil) const;
		void SetRenderTarget(UINT numRenderTargets, ID3D11RenderTargetView* renderTarget[],
			const CDepthStencil* depthStencil) const;
		void SetDepthStencilState(const CDepthStencilState* depthStencilState, UINT stencilRef) const;
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