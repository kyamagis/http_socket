
#include "../../includes/response_utils.hpp"

namespace response_utils
{
#define NOT_FOUND -1
	str_ openReadErrorPage(str_ file_path, bool &fail_flg)
	{
		std::ifstream error_page;
		str_ entity_body;

		if (access(file_path.c_str(), F_OK) == NOT_FOUND)
		{
			fail_flg = true;
			return "";
		}
		error_page.open(file_path.c_str());
		if (!error_page.is_open() || !error_page)
		{
			fail_flg = true;
			return "";
		}
		char read_str[10];
		while (!error_page.eof())
		{
			error_page.read(read_str, 10);
			if (error_page.fail() && !error_page.eof())
			{
				fail_flg = true;
				break;
			}
			entity_body.append(read_str, error_page.gcount());
		}
		error_page.close();
		fail_flg = false;
		return entity_body;
	}

	str_ addTypeLengthBody(str_ file_path)
	{
		bool fail_flg;
		str_ entity_body;
		str_ response_message;

		entity_body = openReadErrorPage(file_path, fail_flg);
		if (fail_flg)
		{
			return "\r\n";
		}

		if (utils::contentEachExtension(file_path, ".jpg"))
		{
			response_message = "Content-Type: image/jpg\r\n";
		}
		else if (utils::contentEachExtension(file_path, ".html"))
		{
			response_message = "Content-Type: text/html\r\n";
		}
		else if (utils::contentEachExtension(file_path, ".css"))
		{
			response_message = "Content-Type: text/css\r\n";
		}
		else
		{
			response_message = "Content-Type: text/plain\r\n";
		}
		response_message += "Content-Length: " + utils::to_string(entity_body.size()) + "\r\n\r\n";
		return response_message + entity_body;
	}

	map_status_ setMapStatus()
	{
		map_status_ map_status;

		map_status[200] = "200 OK\r\n";
		map_status[301] = "301 Moved Parmanently\r\n";
		map_status[400] = "400 Bad Request\r\n";
		map_status[403] = "403 Forbidden\r\n";
		map_status[404] = "404 Not Found\r\n";
		map_status[405] = "405 Method Not Allowed\r\n";
		map_status[415] = "415 Unsupported Media Type\r\n";
		map_status[500] = "500 Internal Server Error\r\n";
		map_status[501] = "501 Not Implemented\r\n";
		map_status[505] = "505 HTTP Version Not Supported\r\n";
		map_status[508] = "508 Loop Detected\r\n";

		return map_status;
	}

	map_wday_ setMapWday()
	{
		map_wday_ map_wday;

		map_wday[0] = "Sun";
		map_wday[1] = "Mon";
		map_wday[2] = "Tue";
		map_wday[3] = "Wed";
		map_wday[4] = "Thu";
		map_wday[5] = "Fri";
		map_wday[6] = "Sat";

		return map_wday;
	}

	map_month_ setMapMonth()
	{
		map_month_ map_month;

		map_month[0] = "Jan";
		map_month[1] = "Feb";
		map_month[2] = "Mar";
		map_month[3] = "Apr";
		map_month[4] = "May";
		map_month[5] = "Jun";

		map_month[6] = "Jul";
		map_month[7] = "Aug";
		map_month[8] = "Sep";
		map_month[9] = "Oct";
		map_month[10] = "Nov";
		map_month[11] = "Dec";

		return map_month;
	}
}
