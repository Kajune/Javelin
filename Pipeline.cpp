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

using namespace Javelin;

CPipeline::CPipeline() noexcept :
m_pDeviceContext(nullptr){
}

void CPipeline::Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext) {
	m_pDeviceContext = pDeviceContext;
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

void CPipeline::SetHullShader(const CHullShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->HSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetDomainShader(const CDomainShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->DSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetGeometryShader(const CGeometryShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->GSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
}

void CPipeline::SetComputeShader(const CComputeShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->CSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
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

void CPipeline::SetPixelShader(const CPixelShader* shader) const {
	if (!m_pDeviceContext) {
		Application::WriteLog("パイプラインにデバイスコンテキストがセットされていません");
		return;
	}

	m_pDeviceContext->PSSetShader(shader ? shader->GetShader() : nullptr, nullptr, 0);
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


