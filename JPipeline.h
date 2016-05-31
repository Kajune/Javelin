#pragma once
#include "JGlobal.h"
#include <memory>
#include <functional>
#include <vector>
#include <map>
#include "JVertexAndIndexBuffer.h"

namespace Javelin {
	template <typename BufferType>
	class CConstantBuffer;
	class CInputLayout;
	class CShaderResourceView;
	class CSamplerState;

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
	class CCubeTexture;

	class CPipeline final {
		std::shared_ptr<ID3D11DeviceContext> m_pDeviceContext;
		bool m_useDefaultSettings;

		struct Storage;
		mutable std::unique_ptr<Storage> m_store;

		template<typename BufferType>
		void SetConstantBuffer(UINT ShaderType, UINT slot, const CConstantBuffer<BufferType>* pBuffer) const;
		void SetShaderResource(UINT ShaderType, UINT slot, ID3D11ShaderResourceView* srv) const;
		void SetSampler(UINT ShaderType, UINT slot, ID3D11SamplerState* sampler) const;
	public:
		CPipeline() noexcept;
		~CPipeline() noexcept;

		void Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext,
			bool useDefaultSettings = true);
		void OnDefaultSettingsChanged() const;

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
		void SetVertexShaderSamplerState(UINT slot, const CSamplerState* samplerState) const;

		//HS
		void SetHullShader(const CHullShader* pShader) const;
		template<typename BufferType>
		void SetHullShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetHullShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;
		void SetHullShaderSamplerState(UINT slot, const CSamplerState* samplerState) const;

		//DS
		void SetDomainShader(const CDomainShader* pShader) const;
		template<typename BufferType>
		void SetDomainShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetDomainShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;
		void SetDomainShaderSamplerState(UINT slot, const CSamplerState* samplerState) const;

		//GS
		void SetGeometryShader(const CGeometryShader* pShader) const;
		template<typename BufferType>
		void SetGeometryShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetGeometryShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;
		void SetGeometryShaderSamplerState(UINT slot, const CSamplerState* samplerState) const;

		//CS
		void SetComputeShader(const CComputeShader* pShader) const;
		template<typename BufferType>
		void SetComputeShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetComputeShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;
		void SetComputeShaderSamplerState(UINT slot, const CSamplerState* samplerState) const;

		//SO
		void SetStreamOutputTarget(const CBuffer* buffer, UINT offset) const;
		void SetStreamOutputTarget(UINT numTargets, const CBuffer* buffer[], UINT offset[]) const;

		//RS
		void SetRasterizerState(const CRasterizerState* pRasterizerState) const;
		void SetViewports(const CViewport* viewport) const;
		void SetViewports(UINT numViewports, CViewport const* ppViewports[]) const;

		//PS
		void SetPixelShader(const CPixelShader* pShader) const;
		template<typename BufferType>
		void SetPixelShaderConstantBuffer(UINT slot,
			const CConstantBuffer<BufferType>* pBuffer) const;
		void SetPixelShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const;
		void SetPixelShaderSamplerState(UINT slot, const CSamplerState* samplerState) const;

		//OM
		void SetRenderTarget(const CRenderTarget* renderTarget, const CDepthStencil* depthStencil) const;
		void SetRenderTarget(UINT numRenderTargets, CRenderTarget const* ppRenderTarget[],
			const CDepthStencil* depthStencil) const;
		void SetRenderTarget(UINT numRenderTargets, ID3D11RenderTargetView* ppRenderTarget[],
			ID3D11DepthStencilView* pDepthStencil) const;
		void SetRenderTarget(const CCubeTexture& cubeTexture) const;
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

		//‚»‚Ì‘¼
		void ClearState() const;
		std::shared_ptr<ID3D11DeviceContext> GetDeviceContext() const {
			return m_pDeviceContext;
		}
		void BeginStorePipeline() const noexcept;
		void RestorePipeline() const;
	};

	enum ShaderType : UINT {
		S_VERTEX = 0,
		S_HULL,
		S_DOMAIN,
		S_GEOMETRY,
		S_COMPUTE,
		S_PIXEL,
		S_DUMMY,
	};

	static const std::function<void(ID3D11DeviceContext&, UINT, UINT, ID3D11ShaderResourceView* const*)>
		SetShaderResourceFuncs[] = {
		&ID3D11DeviceContext::VSSetShaderResources,
		&ID3D11DeviceContext::HSSetShaderResources,
		&ID3D11DeviceContext::DSSetShaderResources,
		&ID3D11DeviceContext::GSSetShaderResources,
		&ID3D11DeviceContext::CSSetShaderResources,
		&ID3D11DeviceContext::PSSetShaderResources,
	};

	static const std::function<void(ID3D11DeviceContext&, UINT, UINT, ID3D11ShaderResourceView**)>
		GetShaderResourceFuncs[] = {
		&ID3D11DeviceContext::VSGetShaderResources,
		&ID3D11DeviceContext::HSGetShaderResources,
		&ID3D11DeviceContext::DSGetShaderResources,
		&ID3D11DeviceContext::GSGetShaderResources,
		&ID3D11DeviceContext::CSGetShaderResources,
		&ID3D11DeviceContext::PSGetShaderResources,
	};

	static const std::function<void(ID3D11DeviceContext&, UINT, UINT, ID3D11SamplerState* const*)>
		SetShaderSamplerFuncs[] = {
		&ID3D11DeviceContext::VSSetSamplers,
		&ID3D11DeviceContext::HSSetSamplers,
		&ID3D11DeviceContext::DSSetSamplers,
		&ID3D11DeviceContext::GSSetSamplers,
		&ID3D11DeviceContext::CSSetSamplers,
		&ID3D11DeviceContext::PSSetSamplers,
	};

	static const std::function<void(ID3D11DeviceContext&, UINT, UINT, ID3D11SamplerState**)>
		GetShaderSamplerFuncs[] = {
		&ID3D11DeviceContext::VSGetSamplers,
		&ID3D11DeviceContext::HSGetSamplers,
		&ID3D11DeviceContext::DSGetSamplers,
		&ID3D11DeviceContext::GSGetSamplers,
		&ID3D11DeviceContext::CSGetSamplers,
		&ID3D11DeviceContext::PSGetSamplers,
	};

	static const std::function<void(ID3D11DeviceContext&, UINT, UINT, ID3D11Buffer**)>
		SetConstantBufferFuncs[] = {
		&ID3D11DeviceContext::VSSetConstantBuffers,
		&ID3D11DeviceContext::HSSetConstantBuffers,
		&ID3D11DeviceContext::DSSetConstantBuffers,
		&ID3D11DeviceContext::GSSetConstantBuffers,
		&ID3D11DeviceContext::CSSetConstantBuffers,
		&ID3D11DeviceContext::PSSetConstantBuffers,
	};

	static const std::function<void(ID3D11DeviceContext&, UINT, UINT, ID3D11Buffer**)>
		GetConstantBufferFuncs[] = {
		&ID3D11DeviceContext::VSGetConstantBuffers,
		&ID3D11DeviceContext::HSGetConstantBuffers,
		&ID3D11DeviceContext::DSGetConstantBuffers,
		&ID3D11DeviceContext::GSGetConstantBuffers,
		&ID3D11DeviceContext::CSGetConstantBuffers,
		&ID3D11DeviceContext::PSGetConstantBuffers,
	};

	struct CPipeline::Storage {
		static const UINT kMaxClassInstance = 256;

		ID3D11InputLayout* inputLayout = nullptr;
		bool changed_inputLayout = false;

		typedef struct {
			ID3D11Buffer* buffer;
			UINT stride;
			UINT offset;
		}vbufferSet_t;
		std::map<UINT, vbufferSet_t> vertexBuffer;
		bool changed_vertexBuffer = false;

		ID3D11Buffer* indexBuffer = nullptr;
		DXGI_FORMAT indexFormat;
		UINT indexOffset;
		bool changed_indexBuffer = false;

		D3D11_PRIMITIVE_TOPOLOGY primitive;
		bool changed_primitive = false;

		ID3D11VertexShader* VertexShader = nullptr;
		ID3D11ClassInstance* VertexClassInstance[kMaxClassInstance];
		UINT VertexClassInstanceNum = 0;
		bool changed_vertexShader = false;

		ID3D11HullShader* HullShader = nullptr;
		ID3D11ClassInstance* HullClassInstance[kMaxClassInstance];
		UINT HullClassInstanceNum = 0;
		bool changed_HullShader = false;

		ID3D11DomainShader* DomainShader = nullptr;
		ID3D11ClassInstance* DomainClassInstance[kMaxClassInstance];
		UINT DomainClassInstanceNum = 0;
		bool changed_DomainShader = false;

		ID3D11GeometryShader* GeometryShader = nullptr;
		ID3D11ClassInstance* GeometryClassInstance[kMaxClassInstance];
		UINT GeometryClassInstanceNum = 0;
		bool changed_GeometryShader = false;

		ID3D11ComputeShader* ComputeShader = nullptr;
		ID3D11ClassInstance* ComputeClassInstance[kMaxClassInstance];
		UINT ComputeClassInstanceNum = 0;
		bool changed_ComputeShader = false;

		ID3D11PixelShader* PixelShader = nullptr;
		ID3D11ClassInstance* PixelClassInstance[kMaxClassInstance];
		UINT PixelClassInstanceNum = 0;
		bool changed_PixelShader = false;

		typedef struct {
			std::map<UINT, ID3D11ShaderResourceView*> ShaderResource;
			bool changed_ShaderResource = false;

			std::map<UINT, ID3D11SamplerState*> Sampler;
			bool changed_Sampler = false;

			std::map<UINT, ID3D11Buffer*> ConstantBuffer;
			bool changed_ConstantBuffer = false;
		}ShaderSet_t;
		ShaderSet_t ShaderSet[S_DUMMY];

		std::vector<ID3D11Buffer*> StreamOutput;
		bool changed_StreamOutput = false;

		ID3D11RasterizerState* Rasterizer = nullptr;
		bool changed_Rasterizer = false;

		std::vector<D3D11_VIEWPORT> Viewport;
		bool changed_Viewport = false;

		std::vector<ID3D11RenderTargetView*> RenderTarget;
		ID3D11DepthStencilView* DepthStencil = nullptr;
		bool changed_RenderTarget = false;

		ID3D11DepthStencilState* DepthStencilState = nullptr;
		UINT Stencil;
		bool changed_DepthStencilState = false;

		ID3D11BlendState* BlendState = nullptr;
		COLOR BlendColor;
		UINT SampleMask;
		bool changed_BlendState = false;
	};
}


