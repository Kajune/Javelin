#pragma once

#include "JResource.h"

namespace Javelin {
	class CShaderResourceView : public CResource {
	public:
		virtual ID3D11ShaderResourceView* GetShaderResourceView() const noexcept = 0;
	};
}
