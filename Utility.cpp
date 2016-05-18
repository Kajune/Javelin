#include "JUtility.h"

#include <memory>
#include <sstream>

std::string Javelin::wstr_to_str(const std::wstring& wstr) {
	size_t convertedNum;
	wcstombs_s(&convertedNum, nullptr, 0, wstr.c_str(), _TRUNCATE);
	std::unique_ptr<char[]> ch(new char[convertedNum]);
	wcstombs_s(nullptr, ch.get(), sizeof(char) * convertedNum, wstr.c_str(), _TRUNCATE);
	return std::string(ch.get());
}

std::wstring Javelin::str_to_wstr(const std::string& str) {
	size_t convertedNum;
	mbstowcs_s(&convertedNum, nullptr, 0, str.c_str(), _TRUNCATE);
	std::unique_ptr<wchar_t[]> wch(new wchar_t[convertedNum]);
	mbstowcs_s(nullptr, wch.get(), sizeof(char) * convertedNum, str.c_str(), _TRUNCATE);
	return std::wstring(wch.get());
}

void Javelin::Split(const std::string& str,
	std::vector<std::string>& splitedStr,
	const std::string& delim,
	bool allowEmptyElement) {
	size_t found, current = 0;
	while ((found = str.find(delim, current)) != std::string::npos) {
		if (current != found || allowEmptyElement) {
			splitedStr.push_back(str.substr(current, found - current));
		}
		current = found + delim.size();
	}
	if (current != found || allowEmptyElement) {
		splitedStr.push_back(str.substr(current, found - current));
	}
}

void Javelin::Split(const std::string& str,
	std::vector<std::string>& splitedStr,
	char delim, bool allowEmptyElement) {
	size_t current = 0, found;
	while ((found = str.find_first_of(delim, current)) != std::string::npos) {
		if (current != found > 0 || allowEmptyElement) {
			splitedStr.emplace_back(str, current, found - current);
		}
		current = found + 1;
	}
	if (current != found > 0 || allowEmptyElement) {
		splitedStr.emplace_back(str, current, str.size() - current);
	}
}

