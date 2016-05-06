#pragma once

#include "JGlobal.h"

namespace Javelin {

	class CResource {
	public:
		CResource() = default;
		virtual ~CResource() = default;
		CResource(const CResource&) = delete;
		CResource(CResource&&) = delete;
		CResource& operator=(const CResource&) = delete;
		CResource& operator=(CResource&&) = delete;
	};

}
