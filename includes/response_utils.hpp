#ifndef response_utils_hpp
# define response_utils_hpp

#include "./utils.hpp"

namespace response_utils
{
	str_		openReadErrorPage(str_ file_path, bool &fail_flg);
	str_		addTypeLengthBody(str_ file_path);
	map_status_	setMapStatus();
	map_wday_	setMapWday();
	map_month_	setMapMonth();
}

#endif
