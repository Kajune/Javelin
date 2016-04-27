#pragma once

#include "JBuffer.h"
#include <memory>

namespace Javelin {

	template <typename BufferType>
	class CConstantBuffer : public CBuffer {
		static_assert(sizeof(BufferType) % 16 == 0, 
			"�萔�o�b�t�@�̃T�C�Y��16�̔{���ł���K�v������܂�");
		static_assert(std::alignment_of<BufferType>::value == 4,
			"�萔�o�b�t�@�͂S�o�C�g�A���C�����g�ł���K�v������܂�");
		static_assert(sizeof(BufferType) <= 4096 * 16,
			"�萔�o�b�t�@���傫�����܂�");
	public:
		void Initialize();

		void UpdateBufferValue(const BufferType& buffer, 
			std::shared_ptr<ID3D11DeviceContext> pDeviceContext);

		using m_bufferType = BufferType;
	};

}
