#include "JBlendState.h"
#include "JApplication.h"

using namespace Javelin;

CBlendState::CBlendState() noexcept :
m_pBlendState(nullptr) {
	m_BlendDesc.AlphaToCoverageEnable = false;
	m_BlendDesc.IndependentBlendEnable = false;
	for (UINT i = 0; i < array_length(m_BlendDesc.RenderTarget); i++) {
		m_BlendDesc.RenderTarget[i].BlendEnable = false;
		m_BlendDesc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		m_BlendDesc.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		m_BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		m_BlendDesc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		m_BlendDesc.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		m_BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		m_BlendDesc.RenderTarget[i].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
	}
}

CBlendState::~CBlendState() noexcept {
	Cleanup();
}

/*
CBlendState::CBlendState(const CBlendState& BlendState) {
	operator=(BlendState);
}

CBlendState& CBlendState::operator=(const CBlendState& BlendState) {
	if (this == &BlendState) {
		return *this;
	}
	m_BlendDesc = BlendState.m_BlendDesc;
	Initialize();
	return *this;
}

CBlendState::CBlendState(CBlendState&& BlendState) {
	operator=(std::move(BlendState));
}

CBlendState& CBlendState::operator=(CBlendState&& BlendState) {
	if (this == &BlendState) {
		return *this;
	}
	Cleanup();
	m_BlendDesc = BlendState.m_BlendDesc;
	m_pBlendState = BlendState.m_pBlendState;
	return *this;
}*/

void CBlendState::Initialize() {
	Cleanup();

	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateBlendState(&m_BlendDesc, &m_pBlendState))) {
			Application::WriteLog("�f�v�X�X�e���V���X�e�[�g�̍쐬�Ɏ��s���܂���");
			throw - 1;
		}
	} else {
		Application::WriteLog("�f�o�C�X��������܂���ł���");
		throw - 1;
	}
}

void CBlendState::Cleanup() noexcept {
	SAFE_RELEASE(m_pBlendState);
}

void CBlendState::SetBlendDesc(const D3D11_BLEND_DESC& blendDesc) noexcept{
	m_BlendDesc = blendDesc;
}

void CBlendState::SetEnableBlend(bool value, UINT renderTarget) noexcept {
	if (renderTarget >= GetBlendRenderTargetNum()) {
		Application::WriteLog("CBlendState::SetEnableBlend renderTarget�̒l���s���ł�");
		return;
	}
	m_BlendDesc.RenderTarget[renderTarget].BlendEnable = value;
}

void CBlendState::SetAlignmentBlend(const COLOR& color, UINT renderTarget) noexcept {
	if (renderTarget >= GetBlendRenderTargetNum()) {
		Application::WriteLog("CBlendState::SetAlignmentBlend renderTarget�̒l���s���ł�");
		return;
	}
	m_BlendDesc.RenderTarget[renderTarget].BlendEnable = true;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[renderTarget].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	m_BlendDesc.RenderTarget[renderTarget].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].BlendOpAlpha = D3D11_BLEND_OP_ADD;
}

void CBlendState::SetAddBlend(const COLOR& color, UINT renderTarget) noexcept {
	if (renderTarget >= GetBlendRenderTargetNum()) {
		Application::WriteLog("CBlendState::SetAddBlend renderTarget�̒l���s���ł�");
		return;
	}
	m_BlendDesc.RenderTarget[renderTarget].BlendEnable = true;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[renderTarget].DestBlend = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].BlendOpAlpha = D3D11_BLEND_OP_ADD;
}

void CBlendState::SetSubtractBlend(const COLOR& color, UINT renderTarget) noexcept {
	if (renderTarget >= GetBlendRenderTargetNum()) {
		Application::WriteLog("CBlendState::SetSubtractBlend renderTarget�̒l���s���ł�");
		return;
	}
	m_BlendDesc.RenderTarget[renderTarget].BlendEnable = true;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	m_BlendDesc.RenderTarget[renderTarget].DestBlend = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].BlendOp = D3D11_BLEND_OP_SUBTRACT;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].BlendOpAlpha = D3D11_BLEND_OP_ADD;
}

void CBlendState::SetMultipleBlend(const COLOR& color, UINT renderTarget) noexcept {
	if (renderTarget >= GetBlendRenderTargetNum()) {
		Application::WriteLog("CBlendState::SetMultipleBlend renderTarget�̒l���s���ł�");
		return;
	}
	m_BlendDesc.RenderTarget[renderTarget].BlendEnable = true;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlend = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].DestBlend = D3D11_BLEND_SRC_COLOR;
	m_BlendDesc.RenderTarget[renderTarget].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].BlendOpAlpha = D3D11_BLEND_OP_ADD;
}

void CBlendState::SetFactorBlend(const COLOR& color, UINT renderTarget) noexcept {
	if (renderTarget >= GetBlendRenderTargetNum()) {
		Application::WriteLog("CBlendState::SetFactorBlend renderTarget�̒l���s���ł�");
		return;
	}
	m_BlendDesc.RenderTarget[renderTarget].BlendEnable = true;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	m_BlendDesc.RenderTarget[renderTarget].DestBlend = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].BlendOp = D3D11_BLEND_OP_ADD;
	m_BlendDesc.RenderTarget[renderTarget].SrcBlendAlpha = D3D11_BLEND_ONE;
	m_BlendDesc.RenderTarget[renderTarget].DestBlendAlpha = D3D11_BLEND_ZERO;
	m_BlendDesc.RenderTarget[renderTarget].BlendOpAlpha = D3D11_BLEND_OP_ADD;
}

void CBlendState::ApplyChange() {
	Initialize();
}
