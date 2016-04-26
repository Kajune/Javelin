#include "JRasterizerState.h"
#include "JUtility.h"
#include "JApplication.h"

using namespace Javelin;

CRasterizerState::CRasterizerState() noexcept :
m_pRasterizerState(nullptr){
}

CRasterizerState::~CRasterizerState() noexcept {
	Cleanup();
}

CRasterizerState::CRasterizerState(const CRasterizerState& rasterizerState) {
	operator=(rasterizerState);
}

CRasterizerState& CRasterizerState::operator=(const CRasterizerState& rasterizerState) {
	if (this == &rasterizerState) {
		return *this;
	}
	Initialize(rasterizerState.m_rasterizerDesc);
	return *this;
}

CRasterizerState::CRasterizerState(CRasterizerState&& rasterizerState) {
	operator=(std::move(rasterizerState));
}

CRasterizerState& CRasterizerState::operator=(CRasterizerState&& rasterizerState) {
	if (this == &rasterizerState) {
		return *this;
	}
	Cleanup();
	m_pRasterizerState = rasterizerState.m_pRasterizerState;
	m_rasterizerDesc = rasterizerState.m_rasterizerDesc;
	return *this;
}

void CRasterizerState::Initialize(const D3D11_RASTERIZER_DESC& rsDesc) {
	Cleanup();

	m_rasterizerDesc = rsDesc;
	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateRasterizerState(&m_rasterizerDesc, &m_pRasterizerState))) {
			Application::WriteLog("ラスタライザステートの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CRasterizerState::ApplyChange() {
	Initialize(m_rasterizerDesc);
}

void CRasterizerState::Cleanup() noexcept {
	SAFE_RELEASE(m_pRasterizerState);
}
