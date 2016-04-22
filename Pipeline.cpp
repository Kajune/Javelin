#include "JPipeline.h"

using namespace Javelin;

CPipeline::CPipeline() noexcept :
m_pDeviceContext(nullptr){
}

void CPipeline::Initialize(std::shared_ptr<ID3D11DeviceContext> pDeviceContext) {
	m_pDeviceContext = pDeviceContext;
}



