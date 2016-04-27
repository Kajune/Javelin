#pragma once

#include "JBuffer.h"
#include <memory>

namespace Javelin {

	template <typename BufferType>
	class CConstantBuffer : public CBuffer {
		static_assert(sizeof(BufferType) % 16 == 0, 
			"定数バッファのサイズは16の倍数である必要があります");
		static_assert(std::alignment_of<BufferType>::value == 4,
			"定数バッファは４バイトアライメントである必要があります");
		static_assert(sizeof(BufferType) <= 4096 * 16,
			"定数バッファが大きすぎます");
	public:
		void Initialize();

		void UpdateBufferValue(const BufferType& buffer, 
			std::shared_ptr<ID3D11DeviceContext> pDeviceContext);

		using m_bufferType = BufferType;
	};

}
