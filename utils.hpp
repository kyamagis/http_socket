#ifndef utils_hpp
# define utils_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <cerrno>
#include <cstring>
#include <vector>

#define str_ std::string
#define vec_str_ std::vector<str_>

enum e_phase
{
	RECV_REQUEST,
	RECV_ENTITY_BODY,
	SEND_RESPONSE,
	CGI_PHASE,
	CLOSE_ACCEPTED_SOCKET
};

namespace utils
{
	template <class T> str_ to_string(const T& target)
	{
		std::ostringstream oss;

		oss << target;
		return oss.str();
	}

	vec_str_	splitStr(const str_ &str, const str_ &delim);

	void	putError(str_ error_str);
	void	exitWithPutError(str_ error_str);
	str_ 	intToStr(int num);
}

#endif