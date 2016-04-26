#include "JInputLayout.h"
#include "JApplication.h"
#include "JUtility.h"

using namespace Javelin;

CInputLayout::CInputLayout() noexcept :
m_pInputLayout(nullptr){
}

CInputLayout::~CInputLayout() noexcept {
	Cleanup();
}

CInputLayout::CInputLayout(CInputLayout&& inputLayout) {
	operator=(std::move(inputLayout));
}

CInputLayout& CInputLayout::operator=(CInputLayout&& inputLayout) {
	if (this == &inputLayout) {
		return *this;
	}

	Cleanup();
	m_pInputLayout = inputLayout.m_pInputLayout;
	return *this;
}

void CInputLayout::Initialize(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs,
	UINT numElements, const CVertexShader& vertexShader) {
	Cleanup();

	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateInputLayout(pInputElementDescs,
			numElements, vertexShader.GetBlob()->GetBufferPointer(), 
			vertexShader.GetBlob()->GetBufferSize(), &m_pInputLayout))) {
			Application::WriteLog("���̓��C�A�E�g�̍쐬�Ɏ��s���܂���");
			throw - 1;
		}
	} else {
		Application::WriteLog("�f�o�C�X��������܂���ł���");
		throw - 1;
	}
}

void CInputLayout::Cleanup() noexcept {
	SAFE_RELEASE(m_pInputLayout);
}
