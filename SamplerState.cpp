#include "JSamplerState.h"
#include "JUtility.h"
#include "JApplication.h"

using namespace Javelin;

CSamplerState::CSamplerState() noexcept :
m_pSamplerState(nullptr) {
}

CSamplerState::~CSamplerState() noexcept {
	Cleanup();
}

/*
CSamplerState::CSamplerState(const CSamplerState& sampler) {
	operator=(sampler);
}

CSamplerState& CSamplerState::operator=(const CSamplerState& sampler) {
	if (this == &sampler) {
		return *this;
	}
	Cleanup();
	Initialize(sampler.m_desc);
	return *this;
}

CSamplerState::CSamplerState(CSamplerState&& sampler) {
	operator=(std::move(sampler));
}

CSamplerState& CSamplerState::operator=(CSamplerState&& sampler) {
	if (this == &sampler) {
		return *this;
	}
	Cleanup();
	m_pSamplerState = sampler.m_pSamplerState;
	return *this;
}*/

void CSamplerState::Initialize(D3D11_TEXTURE_ADDRESS_MODE textureAddress, D3D11_FILTER filter,
	const COLOR& borderColor, UINT maxAnisotropy) {
	Cleanup();

	m_desc.AddressU = m_desc.AddressV = m_desc.AddressW = textureAddress;
	m_desc.Filter = filter;
	m_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	m_desc.MaxAnisotropy = maxAnisotropy;
	for (UINT i = 0; i < 4; i++) {
		m_desc.BorderColor[i] = borderColor.ary.rgba[i];
	}
	m_desc.MipLODBias = 0.0f;
	m_desc.MinLOD = 0.0f;
	m_desc.MaxLOD = D3D11_FLOAT32_MAX;
	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateSamplerState(&m_desc, &m_pSamplerState))) {
			Application::WriteLog("サンプラステートの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CSamplerState::Initialize(const D3D11_SAMPLER_DESC& desc) {
	Cleanup();

	m_desc = desc;
	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateSamplerState(&m_desc, &m_pSamplerState))) {
			Application::WriteLog("サンプラステートの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CSamplerState::Cleanup() noexcept {
	SAFE_RELEASE(m_pSamplerState);
}
