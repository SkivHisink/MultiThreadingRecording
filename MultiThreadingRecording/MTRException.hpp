#include <exception>
#include <string>

class MTRException final :std::exception
{
	std::string m_error = "";
public:
	MTRException() {}
	MTRException(std::string str) :m_error(str) {}
	const char* what() noexcept { return m_error.c_str(); }
};
