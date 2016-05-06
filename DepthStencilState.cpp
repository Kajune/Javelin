#include "JDepthStencilState.h"
#include "JUtility.h"
#include "JApplication.h"

using namespace Javelin;

CDepthStencilState::CDepthStencilState() noexcept :
m_pDepthSteincilState(nullptr) {
}

CDepthStencilState::~CDepthStencilState() noexcept {
	Cleanup();
}

/*
CDepthStencilState::CDepthStencilState(const CDepthStencilState& DepthSteincilState) {
	operator=(DepthSteincilState);
}

CDepthStencilState& CDepthStencilState::operator=(const CDepthStencilState& DepthSteincilState) {
	if (this == &DepthSteincilState) {
		return *this;
	}
	Initialize(DepthSteincilState.m_DepthSteincilDesc);
	return *this;
}

CDepthStencilState::CDepthStencilState(CDepthStencilState&& DepthSteincilState) {
	operator=(std::move(DepthSteincilState));
}

CDepthStencilState& CDepthStencilState::operator=(CDepthStencilState&& DepthSteincilState) {
	if (this == &DepthSteincilState) {
		return *this;
	}
	Cleanup();
	m_pDepthSteincilState = DepthSteincilState.m_pDepthSteincilState;
	m_DepthSteincilDesc = DepthSteincilState.m_DepthSteincilDesc;
	return *this;
}*/

void CDepthStencilState::Initialize(const D3D11_DEPTH_STENCIL_DESC& rsDesc) {
	Cleanup();

	m_DepthSteincilDesc = rsDesc;
	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateDepthStencilState(&m_DepthSteincilDesc, &m_pDepthSteincilState))) {
			Application::WriteLog("デプスステンシルステートの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CDepthStencilState::Cleanup() noexcept {
	SAFE_RELEASE(m_pDepthSteincilState);
}

