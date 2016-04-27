#include "JBuffer.h"
#include "JApplication.h"
#include "JUtility.h"

using namespace Javelin;

CBuffer::CBuffer() noexcept:
m_pBuffer(nullptr) {
}

CBuffer::~CBuffer() noexcept{
	Cleanup();
}

void CBuffer::Initialize(const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* subResource) {
	Cleanup();

	m_desc = desc;
	if (Application::GetDevice()) {
		if (FAILED(Application::GetDevice()->CreateBuffer(&m_desc, subResource, &m_pBuffer))) {
			Application::WriteLog("バッファの作成に失敗しました");
			throw - 1;
		}
	} else {
		Application::WriteLog("デバイスが見つかりませんでした");
		throw - 1;
	}
}

void CBuffer::Cleanup() noexcept {
	SAFE_RELEASE(m_pBuffer);
}

CBuffer::CBuffer(const CBuffer& buffer) {
	operator=(buffer);
}

CBuffer& CBuffer::operator=(const CBuffer& buffer) {
	if (this == &buffer) {
		return *this;
	}
	Cleanup();
	Initialize(buffer.m_desc, nullptr);
	Application::GetImmediateContext()->CopyResource(m_pBuffer, buffer.m_pBuffer);
	return *this;
}

CBuffer::CBuffer(CBuffer&& buffer) {
	operator=(std::move(buffer));
}

CBuffer& CBuffer::operator=(CBuffer&& buffer) {
	if (this == &buffer) {
		return *this;
	}
	Cleanup();
	m_pBuffer = buffer.m_pBuffer;
	m_desc = buffer.m_desc;
	return *this;
}
