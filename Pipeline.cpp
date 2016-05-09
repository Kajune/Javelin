#include "JPipeline.h"
#include <vector>
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

using namespace Javelin;

CPipeline::CPipeline() noexcept :
m_pDeviceContext(nullptr){
}

CPipeline::~CPipeline() noexcept {
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

	m_pDeviceContext->IASetInputLayout(inputLayout ? inputLayout->GetInputLayout() : nullptr);
}

void CPipeline::SetIndexBuffer(const CIndexBuffer* indexBuffer, UINT offset) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
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

	m_pDeviceContext->IASetPrimitiveTopology(topology);
}

void CPipeline::SetVertexShader(const CVertexShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->VSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetVertexShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11ShaderResourceView* srvs[] = { shaderResourceView ?
		shaderResourceView->GetShaderResourceView() : nullptr };
	m_pDeviceContext->VSSetShaderResources(slot, 1, srvs);
}

void CPipeline::SetHullShader(const CHullShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->HSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetHullShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11ShaderResourceView* srvs[] = { shaderResourceView ?
		shaderResourceView->GetShaderResourceView() : nullptr };
	m_pDeviceContext->HSSetShaderResources(slot, 1, srvs);
}

void CPipeline::SetDomainShader(const CDomainShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->DSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetDomainShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11ShaderResourceView* srvs[] = { shaderResourceView ?
		shaderResourceView->GetShaderResourceView() : nullptr };
	m_pDeviceContext->DSSetShaderResources(slot, 1, srvs);
}

void CPipeline::SetGeometryShader(const CGeometryShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->GSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetGeometryShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11ShaderResourceView* srvs[] = { shaderResourceView ?
		shaderResourceView->GetShaderResourceView() : nullptr };
	m_pDeviceContext->GSSetShaderResources(slot, 1, srvs);
}

void CPipeline::SetComputeShader(const CComputeShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->CSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetComputeShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11ShaderResourceView* srvs[] = { shaderResourceView ?
		shaderResourceView->GetShaderResourceView() : nullptr };
	m_pDeviceContext->CSSetShaderResources(slot, 1, srvs);
}

void CPipeline::SetRasterizerState(const CRasterizerState* rasterizerState) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->RSSetState(rasterizerState ? rasterizerState->GetRasterizerState() : nullptr);
}

void CPipeline::SetViewports(const CViewport& viewport) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->RSSetViewports(1, viewport.GetViewport());
}

void CPipeline::SetViewports(UINT numViewports, const CViewport* viewports[]) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
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

	m_pDeviceContext->PSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetPixelShaderResource(UINT slot, const CShaderResourceView* shaderResourceView) const{
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11ShaderResourceView* srvs[] = { shaderResourceView ? 
		shaderResourceView->GetShaderResourceView() : nullptr };
	m_pDeviceContext->PSSetShaderResources(slot, 1, srvs);
}

void CPipeline::SetPixelShaderSamplerState(UINT slot, const CSamplerState* samplerState) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	ID3D11SamplerState* smps[] = { samplerState ? samplerState->GetSamplerState() : nullptr };
	m_pDeviceContext->PSSetSamplers(slot, 1, smps);
}

void CPipeline::SetRenderTarget(const CRenderTarget& renderTarget, const CDepthStencil* depthStencil) const {
	const CRenderTarget* rts[] = { &renderTarget };
	SetRenderTarget(1, rts, depthStencil);
}

void CPipeline::SetRenderTarget(UINT numRenderTargets, CRenderTarget const* renderTarget[],
	const CDepthStencil* depthStencil) const{
	if (numRenderTargets == 1) {
		auto ptr = renderTarget[0]->GetRenderTargetView();
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
	
	m_pDeviceContext->OMSetDepthStencilState(
		depthStencilState ? depthStencilState->GetDepthStencilState() : nullptr, stencilRef);
}

void CPipeline::SetBlendState(const CBlendState* pBlendState, const COLOR& blendFactor, UINT sampleMask) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
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


