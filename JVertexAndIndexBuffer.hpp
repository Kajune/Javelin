#pragma once

#include "JVertexAndIndexBuffer.h"

namespace Javelin {
	template <typename BufferType, UINT BindFlag>
	void CVertexAndIndexBuffer<BufferType, BindFlag>::Initialize(UINT bufferLength, const BufferType* buffer) {
		m_bufferLength = bufferLength;

		D3D11_BUFFER_DESC desc{};
		desc.Usage = buffer ? D3D11_USAGE_IMMUTABLE : D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(BufferType) * bufferLength;
		desc.BindFlags = BindFlag;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subRes{};
		subRes.pSysMem = buffer;
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		CBuffer::Initialize(desc, buffer ? &subRes : nullptr);
	}

	template <typename BufferType, UINT BindFlag>
	void CDynamicVertexAndIndexBuffer<BufferType, BindFlag>::Initialize(UINT bufferLength, const BufferType* buffer) {
		m_bufferLength = bufferLength;

		D3D11_BUFFER_DESC desc{};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(BufferType) * bufferLength;
		desc.BindFlags = BindFlag;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA subRes{};
		subRes.pSysMem = buffer;
		subRes.SysMemPitch = 0;
		subRes.SysMemSlicePitch = 0;
		CBuffer::Initialize(desc, buffer ? &subRes : nullptr);
	}

	template <typename BufferType, UINT BindFlag>
	void CDynamicVertexAndIndexBuffer<BufferType, BindFlag>::WriteBuffer(UINT bufferLength,
		const BufferType* src, std::shared_ptr<ID3D11DeviceContext> pDeviceContext) {
		if (!pDeviceContext) {
			Application::WriteLog("CDynamicVertexAndIndexBuffer::WriteBuffer �f�o�C�X�R���e�L�X�g���Z�b�g����Ă��܂���");
			return;
		}
		D3D11_MAPPED_SUBRESOURCE res;
		if (FAILED(pDeviceContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res))) {
			Application::WriteLog("CDynamicVertexAndIndexBuffer::WriteBuffer ���_�o�b�t�@�̃��b�N�Ɏ��s���܂���");
			throw - 1;
		}

		if (bufferLength > m_bufferLength) {
			Application::WriteLog(std::string("CDynamicBufferBuffer::WriteBuffer ") +
				"�����������Ƃ��Ă���o�b�t�@�͎��ۂɑ��݂���o�b�t�@�T�C�Y���傫�����߁A���ߕ����ǂݎ̂Ă��܂�");
		}

		memcpy_s(res.pData, sizeof(BufferType) * m_bufferLength, src, sizeof(BufferType) * bufferLength);

		pDeviceContext->Unmap(m_pBuffer, 0);
	}



}
