#include "JPipeline.h"
#include <vector>
#include "JApplication.h"
#include "JRenderTarget.h"
#include "JDepthStencil.h"
#include "JDepthStencilState.h"
#include "JRasterizerState.h"
#include "JShader.h"

using namespace Javelin;

CPipeline::CPipeline() noexcept :
m_pDeviceContext(nullptr){
}

void CPipeline::Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext) {
	m_pDeviceContext = pDeviceContext;
}

void CPipeline::SetIndexBuffer(const CIndexBuffer& indexBuffer, UINT offset) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->IASetIndexBuffer(indexBuffer.GetBuffer(),
		sizeof(CIndexBuffer::m_bufferType) == 4 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, offset);
}

void CPipeline::SetVertexShader(const CVertexShader& shader) {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->VSSetShader(shader.GetShader(), nullptr, 0);
}

void CPipeline::SetRasterizerState(const CRasterizerState* rasterizerState) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->RSSetState(rasterizerState ? rasterizerState->GetRasterizerState() : nullptr);
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

void CPipeline::SetRenderTarget(UINT numRenderTargets, const CRenderTarget* renderTarget[],
	const CDepthStencil* depthStencil) const{
	if (numRenderTargets == 1) {
		auto ptr = renderTarget[0]->GetRenderTargetView();
		SetRenderTarget(1, &ptr, depthStencil);
	} else {
		std::vector<ID3D11RenderTargetView*> rtvs;
		rtvs.resize(numRenderTargets);
		for (UINT i = 0; i < numRenderTargets; i++) {
			rtvs.at(i) = renderTarget[i] ? renderTarget[i]->GetRenderTargetView() : nullptr;
		}
		SetRenderTarget(numRenderTargets, rtvs.data(), depthStencil);
	}
}

void CPipeline::SetRenderTarget(UINT numRenderTargets, ID3D11RenderTargetView* renderTarget[],
	const CDepthStencil* depthStencil) const{
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	m_pDeviceContext->OMSetRenderTargets(numRenderTargets, renderTarget, 
		depthStencil ? depthStencil->GetDepthStencilView() : nullptr);
}

void CPipeline::SetDepthStencilState(const CDepthStencilState* depthStencilState, UINT stencilRef) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}
	
	m_pDeviceContext->OMSetDepthStencilState(
		depthStencilState ? depthStencilState->GetDepthStencilState() : nullptr, stencilRef);
}


