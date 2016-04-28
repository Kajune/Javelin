#pragma once
#include "JGlobal.h"
#include <memory>
#include <functional>
#include "JVertexAndIndexBuffer.h"

namespace Javelin {
	template <typename BufferType>
	class CConstantBuffer;
	class CInputLayout;
	class CShaderResourceView;

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

		template<typename BufferType>
		void SetConstantBuffer(UINT slot, const CConstantBuffer<BufferType>* pBuffer, 
			std::function<void(std::shared_ptr<ID3D11DeviceContext>, UINT, UINT, ID3D11Buffer**)> func) const;
	public:
		CPipeline() noexcept;
		~CPipeline() noexcept = default;

		void Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext);

		//IA
		template <typename VertexType>
		void SetVertexBuffer(UINT slot, const CVertexBuffer<VertexType>* pBuffer, 
			UINT stride = 0, UINT offset = 0) const;
		void SetIndexBuffer(const CIndexBuffer* pIndexBuffer, UINT offset) const;
		void SetInputLayout(const CInputLayout* pInputLayout) const;
		void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) const;

		//VS
		void SetVertexShader(const CVertexShader* pShader) const;
		template<typename BufferType>
		void SetVertexShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetVertexShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;

		//HS
		void SetHullShader(const CHullShader* pShader) const;
		template<typename BufferType>
		void SetHullShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetHullShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;

		//DS
		void SetDomainShader(const CDomainShader* pShader) const;
		template<typename BufferType>
		void SetDomainShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetDomainShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;

		//GS
		void SetGeometryShader(const CGeometryShader* pShader) const;
		template<typename BufferType>
		void SetGeometryShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetGeometryShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;

		//CS
		void SetComputeShader(const CComputeShader* pShader) const;
		template<typename BufferType>
		void SetComputeShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetComputeShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;

		//SO

		//RS
		void SetRasterizerState(const CRasterizerState* pRasterizerState) const;
		void SetViewports(UINT numViewports, CViewport const* ppViewports[]) const;

		//PS
		void SetPixelShader(const CPixelShader* pShader) const;
		template<typename BufferType>
		void SetPixelShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetPixelShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;

		//OM
		void SetRenderTarget(UINT numRenderTargets, CRenderTarget const* ppRenderTarget[],
			const CDepthStencil* depthStencil) const;
		void SetRenderTarget(UINT numRenderTargets, ID3D11RenderTargetView* ppRenderTarget[],
			ID3D11DepthStencilView* pDepthStencil) const;
		void SetDepthStencilState(const CDepthStencilState* depthStencilState, UINT stencilRef) const;
		void SetBlendState(const CBlendState* pBlendState, 
			const COLOR& blendFactor = COLOR(0.0f, 0.0f, 0.0f, 0.0f), UINT sampleMask = 0xffffff) const;

		//•`‰æ
		void Draw(UINT numVertex, UINT startVertex = 0) const;
		void DrawIndexed(UINT numIndex, UINT startIndex = 0, int baseVertex = 0) const;
		void DrawInstanced(UINT numVertexPerInstance, UINT numInstance,
						UINT startVertex = 0, UINT startInstance = 0) const;
		void DrawIndexedInstanced(UINT numIndexPerInstance, UINT numInstance,
			UINT startIndex = 0, int baseVertex = 0, UINT startInstance = 0) const;
		void DrawAuto() const;
	};
}