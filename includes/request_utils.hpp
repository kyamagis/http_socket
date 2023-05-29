
#ifndef request_utils_hpp
# define request_utils_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <unistd.h>

#include "./utils.hpp"

typedef	vec_str_::iterator	vec_str_iter_;
typedef	std::map<int, str_> map_status_ ;
typedef	std::map<int, str_> map_wday_ ;
typedef	std::map<int, str_> map_month_ ;

namespace request_utils
{
	size_t		hexStrToLL(const str_ &str, bool &over_flow);
}

#endif
