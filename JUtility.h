#pragma once

#include "JGlobal.h"
#include <vector>

namespace Javelin {
	template<typename T, UINT N>
	constexpr UINT array_length(const T(&)[N]) {
		return N;
	}

	template<typename T>
	void SAFE_RELEASE(T*& x) {
		if (x) {
			x->Release();
			x = nullptr;
		}
	}

	extern std::string wstr_to_str(const std::wstring& wstr);

	extern std::wstring str_to_wstr(const std::string& str);

	extern void Split(const std::string& str,
		std::vector<std::string>& splitedStr,
		const std::string& delim = ",",
		bool allowEmptyElement = false);
	extern void Split(const std::string& str,
		std::vector<std::string>& splitedStr,
		char delim = ',', bool allowEmptyElement = false);

	template<typename T>
	T ceil_to_multiple(const T& value, const T& multiple) {
		return (value + multiple - static_cast<T>(1)) / multiple * multiple;
	}
}
