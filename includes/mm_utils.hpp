
#ifndef mm_utils_hpp
# define mm_utils_hpp

#include <iostream>
#include <string>
#include <deque>
#include <cstring>

#include <arpa/inet.h>

#include "./utils.hpp"

namespace mm_utils
{
	uint16_t	getPortFromAcceptedSocket(int accepted_socket);
}
#endif