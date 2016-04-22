#pragma once
#include "JGlobal.h"
#include <memory>

namespace Javelin {

	class CPipeline final {
		std::shared_ptr<ID3D11DeviceContext> m_pDeviceContext;
	public:
		CPipeline() noexcept;
		~CPipeline() noexcept = default;

		void Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext);
	};

}