#include "JPipeline.h"
#include <limits>
#include "JApplication.h"
#include "JRenderTarget.h"
#include "JDepthStencil.h"
#include "JDepthStencilState.h"
#include "JRasterizerState.h"
#include "JShader.h"
#include "JInputLayout.h"
#include "JBlendState.h"
#include "JShaderResourceView.h"
#include "JSamplerState.h"
#include "JCubeTexture.h"
#include "JBuffer.h"

using namespace Javelin;

CPipeline::CPipeline() noexcept :
m_pDeviceContext(nullptr){
}

CPipeline::~CPipeline() noexcept {
	RestorePipeline();
	Application::ErasePipeline(this);
}

void CPipeline::Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext,
	bool useDefaultSettings) {
	m_pDeviceContext = pDeviceContext;

	m_useDefaultSettings = useDefaultSettings;
	OnDefaultSettingsChanged();

	Application::AddPipeline(this);
}

void CPipeline::OnDefaultSettingsChanged() const {
	if (m_useDefaultSettings) {
		Application::SetDefaultRenderTarget(*this);
		Application::SetDefaultBlendState(*this);
		Application::SetDefaultDepthStencilState(*this);
		Application::SetDefaultRasterizerState(*this);
		Application::SetDefaultViewport(*this);
	}
}

void CPipeline::SetInputLayout(const CInputLayout* inputLayout) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_inputLayout) {
		m_pDeviceContext->IAGetInputLayout(&m_store->inputLayout);
		m_store->changed_inputLayout = true;
	}

	m_pDeviceContext->IASetInputLayout(inputLayout ? inputLayout->GetInputLayout() : nullptr);
}

void CPipeline::SetIndexBuffer(const CIndexBuffer* indexBuffer, UINT offset) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_indexBuffer) {
		m_pDeviceContext->IAGetIndexBuffer(&m_store->indexBuffer, &m_store->indexFormat, &m_store->indexOffset);
		m_store->changed_indexBuffer = true;
	}

	m_pDeviceContext->IASetIndexBuffer(indexBuffer ? indexBuffer->GetBuffer() : nullptr,
		std::numeric_limits<CIndexBuffer::m_bufferType>::digits == 16 ? 
		DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, offset);
}

void CPipeline::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_primitive) {
		m_pDeviceContext->IAGetPrimitiveTopology(&m_store->primitive);
		m_store->changed_primitive = true;
	}

	m_pDeviceContext->IASetPrimitiveTopology(topology);
}

void CPipeline::SetVertexShader(const CVertexShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_vertexShader) {
		m_pDeviceContext->VSGetShader(&m_store->VertexShader, 
			m_store->VertexClassInstance, &m_store->VertexClassInstanceNum);
		m_store->changed_vertexShader = true;
	}

	m_pDeviceContext->VSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetShaderResource(UINT ShaderType, UINT slot, ID3D11ShaderResourceView* srv) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store) {
		if (m_store->ShaderSet[ShaderType].ShaderResource.find(slot) 
			== m_store->ShaderSet[ShaderType].ShaderResource.end()) {
			ID3D11ShaderResourceView* srv;
			GetShaderResourceFuncs[ShaderType](*m_pDeviceContext, slot, 1, &srv);
			m_store->ShaderSet[ShaderType].ShaderResource.emplace(slot, srv);
			m_store->ShaderSet[ShaderType].changed_ShaderResource = true;
		}
	}
	SetShaderResourceFuncs[ShaderType](*m_pDeviceContext, slot, 1, &srv);
}

void CPipeline::SetSampler(UINT ShaderType, UINT slot, ID3D11SamplerState* sampler) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store) {
		if (m_store->ShaderSet[ShaderType].Sampler.find(slot) 
			== m_store->ShaderSet[ShaderType].Sampler.end()) {
			ID3D11SamplerState* sampler;
			GetShaderSamplerFuncs[ShaderType](*m_pDeviceContext, slot, 1, &sampler);
			m_store->ShaderSet[ShaderType].Sampler.emplace(slot, sampler);
			m_store->ShaderSet[ShaderType].changed_Sampler = true;
		}
	}
	SetShaderSamplerFuncs[ShaderType](*m_pDeviceContext, slot, 1, &sampler);
}

void CPipeline::SetVertexShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	SetShaderResource(S_VERTEX, slot, shaderResourceView ? shaderResourceView->GetShaderResourceView() : nullptr);
}

void CPipeline::SetVertexShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	SetSampler(S_VERTEX, slot, samplerState ? samplerState->GetSamplerState() : nullptr);
}

void CPipeline::SetHullShader(const CHullShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_HullShader) {
		m_pDeviceContext->HSGetShader(&m_store->HullShader,
			m_store->HullClassInstance, &m_store->HullClassInstanceNum);
		m_store->changed_HullShader = true;
	}

	m_pDeviceContext->HSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetHullShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	SetShaderResource(S_HULL, slot, shaderResourceView ? shaderResourceView->GetShaderResourceView() : nullptr);
}

void CPipeline::SetHullShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	SetSampler(S_HULL, slot, samplerState ? samplerState->GetSamplerState() : nullptr);
}

void CPipeline::SetDomainShader(const CDomainShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_DomainShader) {
		m_pDeviceContext->DSGetShader(&m_store->DomainShader,
			m_store->DomainClassInstance, &m_store->DomainClassInstanceNum);
		m_store->changed_DomainShader = true;
	}

	m_pDeviceContext->DSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetDomainShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	SetShaderResource(S_DOMAIN, slot, shaderResourceView ? shaderResourceView->GetShaderResourceView() : nullptr);
}

void CPipeline::SetDomainShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	SetSampler(S_DOMAIN, slot, samplerState ? samplerState->GetSamplerState() : nullptr);
}

void CPipeline::SetGeometryShader(const CGeometryShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_GeometryShader) {
		m_pDeviceContext->GSGetShader(&m_store->GeometryShader,
			m_store->GeometryClassInstance, &m_store->GeometryClassInstanceNum);
		m_store->changed_GeometryShader = true;
	}

	m_pDeviceContext->GSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetGeometryShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {	
	SetShaderResource(S_GEOMETRY, slot, shaderResourceView ? shaderResourceView->GetShaderResourceView() : nullptr);
}

void CPipeline::SetGeometryShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	SetSampler(S_GEOMETRY, slot, samplerState ? samplerState->GetSamplerState() : nullptr);
}

void CPipeline::SetComputeShader(const CComputeShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_ComputeShader) {
		m_pDeviceContext->CSGetShader(&m_store->ComputeShader,
			m_store->ComputeClassInstance, &m_store->ComputeClassInstanceNum);
		m_store->changed_ComputeShader = true;
	}

	m_pDeviceContext->CSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetComputeShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	SetShaderResource(S_COMPUTE, slot, shaderResourceView ? shaderResourceView->GetShaderResourceView() : nullptr);
}

void CPipeline::SetComputeShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	SetSampler(S_COMPUTE, slot, samplerState ? samplerState->GetSamplerState() : nullptr);
}

void CPipeline::SetStreamOutputTarget(const CBuffer* buffer, UINT offset) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	SetStreamOutputTarget(1, &buffer, &offset);
}

void CPipeline::SetStreamOutputTarget(UINT numTargets, const CBuffer* buffer[], UINT offset[]) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store) {
		std::vector<ID3D11Buffer*> buf(numTargets);
		m_pDeviceContext->SOGetTargets(numTargets, buf.data());
		for (UINT i = m_store->StreamOutput.size(); i < numTargets;i++) {
			m_store->StreamOutput.push_back(buf.at(i));
		}
		m_store->changed_StreamOutput = true;
	}

	std::vector<ID3D11Buffer*> bufferList;
	for (UINT i = 0; i < numTargets; i++) {
		bufferList.push_back(buffer[i] ? buffer[i]->GetBuffer() : nullptr);
	}
	m_pDeviceContext->SOSetTargets(numTargets, bufferList.data(), offset);
}

void CPipeline::SetRasterizerState(const CRasterizerState* rasterizerState) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_Rasterizer) {
		m_pDeviceContext->RSGetState(&m_store->Rasterizer);
		m_store->changed_Rasterizer = true;
	}

	m_pDeviceContext->RSSetState(rasterizerState ? rasterizerState->GetRasterizerState() : nullptr);
}

void CPipeline::SetViewports(const CViewport* viewport) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	SetViewports(1, &viewport);
}

void CPipeline::SetViewports(UINT numViewports, const CViewport* viewports[]) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_Viewport) {
		UINT num;
		m_pDeviceContext->RSGetViewports(&num, nullptr);
		m_store->Viewport.resize(num);
		m_pDeviceContext->RSGetViewports(&num, m_store->Viewport.data());
		m_store->changed_Viewport = true;
	}

	if (numViewports == 1) {
		m_pDeviceContext->RSSetViewports(1, viewports[0]->GetViewport());
	} else {
		std::vector<D3D11_VIEWPORT> viewportList;
		viewportList.resize(numViewports);
		for (UINT i = 0; i < numViewports; i++) {
			viewportList.at(i) = viewports[i] ? *viewports[i]->GetViewport() : D3D11_VIEWPORT{};
		}
		m_pDeviceContext->RSSetViewports(numViewports, viewportList.data());
	}
}

void CPipeline::SetPixelShader(const CPixelShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store) {
		m_pDeviceContext->PSGetShader(&m_store->PixelShader,
			m_store->PixelClassInstance, &m_store->PixelClassInstanceNum);
		m_store->changed_PixelShader = true;
	}

	m_pDeviceContext->PSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetPixelShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const{
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	SetShaderResource(S_PIXEL, slot, shaderResourceView ? shaderResourceView->GetShaderResourceView() : nullptr);
}

void CPipeline::SetPixelShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	SetSampler(S_PIXEL, slot, samplerState ? samplerState->GetSamplerState() : nullptr);
}

void CPipeline::SetRenderTarget(const CRenderTarget* renderTarget, const CDepthStencil* depthStencil) const {
	const CRenderTarget* rts[] = { renderTarget };
	SetRenderTarget(1, rts, depthStencil);
}

void CPipeline::SetRenderTarget(UINT numRenderTargets, CRenderTarget const* renderTarget[],
	const CDepthStencil* depthStencil) const{
	if (numRenderTargets == 1) {
		auto ptr = renderTarget[0] ? renderTarget[0]->GetRenderTargetView() : nullptr;
		SetRenderTarget(1, &ptr, depthStencil ? depthStencil->GetDepthStencilView() : nullptr);
	} else {
		std::vector<ID3D11RenderTargetView*> rtvs;
		rtvs.resize(numRenderTargets);
		for (UINT i = 0; i < numRenderTargets; i++) {
			rtvs.at(i) = renderTarget[i] ? renderTarget[i]->GetRenderTargetView() : nullptr;
		}
		SetRenderTarget(numRenderTargets, rtvs.data(), depthStencil ? depthStencil->GetDepthStencilView() : nullptr);
	}
}

void CPipeline::SetRenderTarget(UINT numRenderTargets, ID3D11RenderTargetView* renderTarget[],
	ID3D11DepthStencilView* pDepthStencil) const{
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store) {
		if (m_store->changed_RenderTarget) {
			std::vector<ID3D11RenderTargetView*> rtvs(numRenderTargets);
			m_pDeviceContext->OMGetRenderTargets(numRenderTargets, rtvs.data(), nullptr);
			for (UINT i = m_store->RenderTarget.size(); i < rtvs.size(); i++) {
				m_store->RenderTarget.push_back(rtvs.at(i));
			}
		} else {
			std::vector<ID3D11RenderTargetView*> rtvs(numRenderTargets);
			m_pDeviceContext->OMGetRenderTargets(numRenderTargets, rtvs.data(), &m_store->DepthStencil);
			for (UINT i = m_store->RenderTarget.size(); i < rtvs.size(); i++) {
				m_store->RenderTarget.push_back(rtvs.at(i));
			}
			m_store->changed_RenderTarget = true;
		}
	}

	m_pDeviceContext->OMSetRenderTargets(numRenderTargets, renderTarget, pDepthStencil);
}

void CPipeline::SetRenderTarget(const CCubeTexture& cubeTexture) const {
	ID3D11RenderTargetView* rtvs_cube[] = { cubeTexture.GetRenderTargetView() };
	SetRenderTarget(1, rtvs_cube, cubeTexture.GetDepthStencilView());
}

void CPipeline::SetDepthStencilState(const CDepthStencilState* depthStencilState, UINT stencilRef) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_DepthStencilState) {
		m_pDeviceContext->OMGetDepthStencilState(&m_store->DepthStencilState, &m_store->Stencil);
		m_store->changed_DepthStencilState = true;
	}
	
	m_pDeviceContext->OMSetDepthStencilState(
		depthStencilState ? depthStencilState->GetDepthStencilState() : nullptr, stencilRef);
}

void CPipeline::SetBlendState(const CBlendState* pBlendState, const COLOR& blendFactor, UINT sampleMask) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (m_store && !m_store->changed_BlendState) {
		m_pDeviceContext->OMGetBlendState(&m_store->BlendState, m_store->BlendColor.ary.rgba, &m_store->SampleMask);
		m_store->changed_BlendState = true;
	}

	m_pDeviceContext->OMSetBlendState(pBlendState ? pBlendState->GetBlendState() : nullptr, 
		blendFactor.ary.rgba, sampleMask);
}

void CPipeline::Draw(UINT numVertex, UINT startVertex) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->Draw(numVertex, startVertex);
}

void CPipeline::DrawIndexed(UINT numIndex, UINT startIndex, int baseVertex) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->DrawIndexed(numIndex, startIndex, baseVertex);
}

void CPipeline::DrawInstanced(UINT numVertexPerInstance, UINT numInstance,
	UINT startVertex, UINT startInstance) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->DrawInstanced(numVertexPerInstance, numInstance, startVertex, startInstance);
}

void CPipeline::DrawIndexedInstanced(UINT numIndexPerInstance, UINT numInstance,
	UINT startIndex, int baseVertex, UINT startInstance) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->DrawIndexedInstanced(numIndexPerInstance, numInstance,
		startIndex, baseVertex, startInstance);
}

void CPipeline::DrawAuto() const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->DrawAuto();
}

void CPipeline::ClearState() const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->ClearState();
}

void CPipeline::BeginStorePipeline() const noexcept {
	m_store.reset(new Storage);
}

void CPipeline::RestorePipeline() const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	if (!m_store) {
		return;
	}
	if (m_store->changed_inputLayout) {
		m_pDeviceContext->IASetInputLayout(m_store->inputLayout);
		SAFE_RELEASE(m_store->inputLayout);
	}
	if (m_store->changed_vertexBuffer) {
		for (auto& it : m_store->vertexBuffer) {
			m_pDeviceContext->IASetVertexBuffers(it.first, 1, &it.second.buffer, 
				&it.second.stride, &it.second.offset);
			SAFE_RELEASE(it.second.buffer);
		}
	}
	if (m_store->changed_indexBuffer) {
		m_pDeviceContext->IASetIndexBuffer(m_store->indexBuffer,
			m_store->indexFormat, m_store->indexOffset);
		SAFE_RELEASE(m_store->indexBuffer);
	}
	if (m_store->changed_primitive) {
		m_pDeviceContext->IASetPrimitiveTopology(m_store->primitive);
	}
	if (m_store->changed_vertexShader) {
		m_pDeviceContext->VSSetShader(m_store->VertexShader, 
			m_store->VertexClassInstance, m_store->VertexClassInstanceNum);
		SAFE_RELEASE(m_store->VertexShader);
		for (UINT i = 0; i < m_store->VertexClassInstanceNum; i++) {
			SAFE_RELEASE(m_store->VertexClassInstance[i]);
		}
	}
	if (m_store->changed_HullShader) {
		m_pDeviceContext->HSSetShader(m_store->HullShader,
			m_store->HullClassInstance, m_store->HullClassInstanceNum);
		SAFE_RELEASE(m_store->HullShader);
		for (UINT i = 0; i < m_store->HullClassInstanceNum; i++) {
			SAFE_RELEASE(m_store->HullClassInstance[i]);
		}
	}
	if (m_store->changed_DomainShader) {
		m_pDeviceContext->DSSetShader(m_store->DomainShader,
			m_store->DomainClassInstance, m_store->DomainClassInstanceNum);
		SAFE_RELEASE(m_store->DomainShader);
		for (UINT i = 0; i < m_store->DomainClassInstanceNum; i++) {
			SAFE_RELEASE(m_store->DomainClassInstance[i]);
		}
	}
	if (m_store->changed_GeometryShader) {
		m_pDeviceContext->GSSetShader(m_store->GeometryShader,
			m_store->GeometryClassInstance, m_store->GeometryClassInstanceNum);
		SAFE_RELEASE(m_store->GeometryShader);
		for (UINT i = 0; i < m_store->GeometryClassInstanceNum; i++) {
			SAFE_RELEASE(m_store->GeometryClassInstance[i]);
		}
	}
	if (m_store->changed_ComputeShader) {
		m_pDeviceContext->CSSetShader(m_store->ComputeShader,
			m_store->ComputeClassInstance, m_store->ComputeClassInstanceNum);
		SAFE_RELEASE(m_store->ComputeShader);
		for (UINT i = 0; i < m_store->ComputeClassInstanceNum; i++) {
			SAFE_RELEASE(m_store->ComputeClassInstance[i]);
		}
	}
	if (m_store->changed_PixelShader) {
		m_pDeviceContext->PSSetShader(m_store->PixelShader,
			m_store->PixelClassInstance, m_store->PixelClassInstanceNum);
		SAFE_RELEASE(m_store->PixelShader);
		for (UINT i = 0; i < m_store->PixelClassInstanceNum;i++) {
			SAFE_RELEASE(m_store->PixelClassInstance[i]);
		}
	}
	UINT index = 0;
	for (auto& it : m_store->ShaderSet) {
		if (it.changed_ShaderResource) {
			for (auto& it2 : it.ShaderResource) {
				SetShaderResourceFuncs[index](*m_pDeviceContext, it2.first, 1, &it2.second);
				SAFE_RELEASE(it2.second);
			}
		}
		if (it.changed_Sampler) {
			for (auto& it2 : it.Sampler) {
				SetShaderSamplerFuncs[index](*m_pDeviceContext, it2.first, 1, &it2.second);
				SAFE_RELEASE(it2.second);
			}
		}
		if (it.changed_ConstantBuffer) {
			for (auto& it2 : it.ConstantBuffer) {
				SetConstantBufferFuncs[index](*m_pDeviceContext, it2.first, 1, &it2.second);
				SAFE_RELEASE(it2.second);
			}
		}
		index++;
	}
	if (m_store->changed_StreamOutput) {
		std::vector<UINT> offsets(m_store->StreamOutput.size());
		m_pDeviceContext->SOSetTargets(m_store->StreamOutput.size(), 
			m_store->StreamOutput.data(), offsets.data());
		for (auto& it : m_store->StreamOutput) {
			SAFE_RELEASE(it);
		}
	}
	if (m_store->changed_Rasterizer) {
		m_pDeviceContext->RSSetState(m_store->Rasterizer);
		SAFE_RELEASE(m_store->Rasterizer);
	}
	if (m_store->changed_Viewport) {
		m_pDeviceContext->RSSetViewports(m_store->Viewport.size(), m_store->Viewport.data());
	}
	if (m_store->changed_RenderTarget) {
		m_pDeviceContext->OMSetRenderTargets(m_store->RenderTarget.size(), 
			m_store->RenderTarget.data(), m_store->DepthStencil);
		for (auto& it : m_store->RenderTarget) {
			SAFE_RELEASE(it);
		}
		SAFE_RELEASE(m_store->DepthStencil);
	}
	if (m_store->changed_DepthStencilState) {
		m_pDeviceContext->OMSetDepthStencilState(m_store->DepthStencilState, m_store->Stencil);
		SAFE_RELEASE(m_store->DepthStencilState);
	}
	if (m_store->changed_BlendState) {
		m_pDeviceContext->OMSetBlendState(m_store->BlendState, m_store->BlendColor.ary.rgba, m_store->SampleMask);
		SAFE_RELEASE(m_store->BlendState);
	}
	m_store.reset();
}

