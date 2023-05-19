
#include "../includes/utils.hpp"

#define vec_str_ std::vector<str_>

namespace utils
{
	vec_str_ split_Str(const str_ &str, const str_ &delim)
	{
		vec_str_	vec_split_str;
		size_t		head_pos = 0;
		size_t		found_pos = 0;

		while (true)
		{
			found_pos = str.find(delim, head_pos);
			if (found_pos == str_::npos)
			{
				break;
			}
			vec_split_str.push_back(str.substr(head_pos, found_pos - head_pos));
			head_pos = found_pos + delim.size();
		}
		vec_split_str.push_back(str.substr(head_pos));
		return vec_split_str;
	}

	vec_str_ splitStr(const str_ &str, const str_ &delim)
	{
		vec_str_ vec_str;
		size_t pos = 0;
		size_t found;

		while (true)
		{
			found = str.find_first_not_of(delim, pos);
			if (found == str_::npos)
				break;
			pos = found;
			found = str.find_first_of(delim, pos);
			if (found == str_::npos)
				break;
			vec_str.push_back(str.substr(pos, found - pos));
			pos = found + 1;
		}
		vec_str.push_back(str.substr(pos));
		return (vec_str);
	}

	void	putError(str_ error_str)
	{
		std::cerr << error_str << ": " << strerror(errno) << std::endl;
	}

	void	exitWithPutError(str_ error_str)
	{
		putError(error_str);
		std::exit(EXIT_FAILURE);
	}

	void exitWithError(const str_ &error_message)
	{
		std::cerr << error_message << std::endl;
		std::exit(EXIT_FAILURE);
	}

	str_ intToStr(int num)
	{
		std::ostringstream oss;

		oss << num;
		if (num < 10)
		{
			return "0" + oss.str();
		}
		return oss.str();
	}

	void	x_close(int serv_socket, int line)
	{
		int error_flg = close(serv_socket);
		if (error_flg == -1)
		{
			std::cerr << "close() failed : " 
						<< strerror(errno) 
						<< " : line = " << line << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}

	void	x_close(int serv_socket)
	{
		int error_flg = close(serv_socket);
		if (error_flg == -1)
		{
			utils::exitWithPutError("close() failed");
		}
	}
}

namespace parse_utils
{
	void deleteSemicolomn(vec_str_ &split_str)
	{
		size_t size;
		size_t pos_semicolomn;

		size = split_str.size();
		if (size == 0)
			utils::exitWithError("error occurred while reading context");
		pos_semicolomn = split_str[size - 1].find(";");
		if (pos_semicolomn == str_::npos)
			utils::exitWithError("found missing \';\' in context");
		split_str[size - 1] = split_str[size - 1].substr(0, pos_semicolomn);
	}

	const char *InvalidSyntaxException::what() const throw()
	{
		return ("invalid syntax");
	}

	const char *OverflowException::what() const throw()
	{
		return ("value overflow");
	}
}

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
		response_message = "Content-Type: text/plane\r\n";
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


