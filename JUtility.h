#pragma once

#include "JGlobal.h"
#include <memory>
#include <sstream>

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

	static std::string wstr_to_str(const std::wstring& wstr) {
		size_t convertedNum;
		wcstombs_s(&convertedNum, nullptr, 0, wstr.c_str(), _TRUNCATE);
		std::unique_ptr<char[]> ch(new char[convertedNum]);
		wcstombs_s(nullptr, ch.get(), sizeof(char) * convertedNum, wstr.c_str(), _TRUNCATE);
		return std::string(ch.get());
	}

	static std::wstring str_to_wstr(const std::string& str) {
		size_t convertedNum;
		mbstowcs_s(&convertedNum, nullptr, 0, str.c_str(), _TRUNCATE);
		std::unique_ptr<wchar_t[]> wch(new wchar_t[convertedNum]);
		mbstowcs_s(nullptr, wch.get(), sizeof(char) * convertedNum, str.c_str(), _TRUNCATE);
		return std::wstring(wch.get());
	}
}
