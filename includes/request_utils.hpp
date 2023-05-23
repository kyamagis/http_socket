
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
	bool		isAtStrLast(const str_& str, const str_& to_find);
	str_		joinPath(const str_& path_front, const str_& path_back);
	str_		createUniqueFileName(const str_ &file_path, const str_& content_type);
	int			overwriteFile(const str_ &file_content, const str_ &file_name);
	int			makeAndPutFile(const str_ &file_content, const str_ &file_name);
	str_		discriminateExtension(const str_& contents_path);
	str_		replaceAllFormStr(const str_ &raw_str, const str_ &str_to_be_replaced, const str_ &replacement_str);
}

#endif
