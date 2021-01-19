#pragma once
#include <string>

class GUIException final :std::exception
{
	std::string m_error = "";
public:
	GUIException() {}
	GUIException(std::string error) :m_error(error) {}
	const char* what() noexcept { return m_error.c_str(); }
};