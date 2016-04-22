#include "JDepthStencil.h"
#include "JUtility.h"
#include "JApplication.h"

using namespace Javelin;

CDepthSteincilState::CDepthSteincilState() noexcept :
m_pDepthSteincilState(nullptr) {
}

CDepthSteincilState::~CDepthSteincilState() noexcept {
	Cleanup();
}

CDepthSteincilState::CDepthSteincilState(const CDepthSteincilState& DepthSteincilState) {
	operator=(DepthSteincilState);
}

CDepthSteincilState& CDepthSteincilState::operator=(const CDepthSteincilState& DepthSteincilState) {
	if (this == &DepthSteincilState) {
		return *this;
	}
	Initialize(DepthSteincilState.m_DepthSteincilDesc);
	return *this;
}

CDepthSteincilState::CDepthSteincilState(CDepthSteincilState&& DepthSteincilState) {
	operator=(std::move(DepthSteincilState));
}

CDepthSteincilState& CDepthSteincilState::operator=(CDepthSteincilState&& DepthSteincilState) {
	if (this == &DepthSteincilState) {
		return *this;
	}
	m_pDepthSteincilState = DepthSteincilState.m_pDepthSteincilState;
	m_DepthSteincilDesc = DepthSteincilState.m_DepthSteincilDesc;
	return *this;
}

void CDepthSteincilState::Initialize(const D3D11_DEPTH_STENCIL_DESC& rsDesc) {
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

void CDepthSteincilState::Cleanup() noexcept {
	SAFE_RELEASE(m_pDepthSteincilState);
}

