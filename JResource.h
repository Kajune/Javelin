#pragma once

#include "JGlobal.h"

namespace Javelin {

	class CResource {
	public:
		CResource() = default;
		virtual ~CResource() = default;
		CResource(const CResource&) = default;
		CResource(CResource&&) = default;
		CResource& operator=(const CResource&) = default;
		CResource& operator=(CResource&&) = default;
	};

}
