#ifndef method_utils_hpp
# define method_utils_hpp

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

namespace method_utils
{
	bool		isAtStrLast(const str_& str, const str_& to_find);
	str_		createUniqueFileName(const str_ &file_path, const str_& content_type);
	str_		eraseHeadDot(const str_ &str);
	int			overwriteFile(const str_ &file_content, const str_ &file_name);
	int			makeAndPutFile(const str_ &file_content, const str_ &file_name);
	str_		discriminateExtension(const str_& contents_path);
	str_		replaceAllFormStr(const str_ &raw_str, const str_ &str_to_be_replaced, const str_ &replacement_str);
}

#endif
