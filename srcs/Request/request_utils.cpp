
#include "../../includes/request_utils.hpp"

#include <climits>
#include <ctime>
#include <fstream>


namespace request_utils
{
	str_ ft_strlwr(const str_ &str)
	{
		str_ l_str = str;

		for (size_t i = 0; i < l_str.size(); i++)
		{
			l_str[i] = std::tolower(l_str[i]);
		}
		return l_str;
	}

	static bool isNumber(int c)
	{
		if ('0' <= c && c <= '9')
		{
			return true;
		}
		return false;
	}

	static long checkMax(unsigned long num, int flag, size_t digcount)
	{
		if (flag == 1 && 10 < digcount)
		{
			return -1;
		}
		if (flag == 1 && UINT_MAX <= num)
		{
			return -1;
		}
		if (flag == -1)
		{
			return -1;
		}
		return (num * flag);
	}

	long myStrToLL(str_ str)
	{
		unsigned long num = 0;
		size_t i = 0;
		int flag = 1;
		size_t digcount = 1;

		if (str[i] == '-')
		{
			flag = -1;
			i++;
		}
		while (isNumber(str[i]))
		{
			if (num != 0)
				digcount++;
			num = 10 * num + (str[i] - '0');
			i++;
		}
		if (str[i] || (!str[i] && i == 0))
		{
			return -1;
		}
		return (checkMax(num, flag, digcount));
	}

	static bool isHex(int c)
	{
		if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
		{
			return true;
		}
		return false;
	}

	static size_t checkHexMax(unsigned long num, int flag, size_t digcount, bool &over_flow)
	{
		if (flag == 1 && 19 < digcount)
		{
			over_flow = true;
			return -1;
		}
		if (flag == 1 && ULONG_MAX <= num)
		{
			over_flow = true;
			return -1;
		}
		if (flag == -1)
		{
			over_flow = true;
			return -1;
		}
		return (num * flag);
	}

	static int hexToNum(int c)
	{
		switch (c)
		{
			case 'a':
			case 'A':
				return 10;
			case 'b':
			case 'B':
				return 11;
			case 'c':
			case 'C':
				return 12;
			case 'd':
			case 'D':
				return 13;
			case 'e':
			case 'E':
				return 14;
			case 'f':
			case 'F':
				return 15;
			default:
				return c - '0';
		}
	}

	size_t hexStrToLL(const str_ &str, bool &over_flow)
	{
		size_t 	num = 0;
		int		flag = 1;
		size_t	digcount = 1;
		size_t	i = 0;

		if (!isHex(str[i]))
		{
			over_flow = true;
			return -1;
		}
		while (isHex(str[i]))
		{
			if (num != 0)
				digcount++;
			num = 16 * num + hexToNum(str[i]);
			i++;
		}
		if ((str[i]) || (!str[i] && i == 0))
		{
			over_flow = true;
			return -1;
		}
		return (checkHexMax(num, flag, digcount, over_flow));
	}

	bool isAtStrLast(const str_ &str, const str_ &to_find)
	{
		size_t pos_to_find;

		pos_to_find = str.find_last_of(to_find);
		if (pos_to_find == str_::npos)
			return (false);
		else if (pos_to_find + 1 != str.length())
			return (false);
		return (true);
	}

	str_ joinPath(const str_ &path_front, const str_ &path_back)
	{
		str_ full_path = path_front + path_back;

		size_t pos_double_slash = full_path.find("//");
		if (pos_double_slash != str_::npos && pos_double_slash == path_front.length() - 1)
			full_path.erase(pos_double_slash, 1);
		return (full_path);
	}

	static str_ getGMTYYYYMMDDHHMMSS()
	{
		std::time_t now = std::time(NULL);
		if (now == -1)
		{
			return "std::time() fail";
		}
		std::tm *gmt = std::gmtime(&now);
		if (gmt == NULL)
		{
			return "std::gmtime() fail";
		}
		str_ date;

		date = utils::intToStr(gmt->tm_year + 1900) + "_" + 
				utils::intToStr(gmt->tm_mon) + "_" + 
				utils::intToStr(gmt->tm_mday) + "_" + 
				utils::intToStr(gmt->tm_hour) + ':' + 
				utils::intToStr(gmt->tm_min) + ':' + 
				utils::intToStr(gmt->tm_sec);
		return date;
	}

	static str_ convertContentTypeToExtension(const str_ &content_type)
	{
		if (content_type == "text/html")
			return ".html";
		if (content_type == "text/css")
			return ".css";
		if (content_type == "image/jpg")
			return ".jpg";
		return ".txt";
	}

#define NOT_FOUND -1

	str_ createUniqueFileName(const str_ &file_path, const str_ &content_type)
	{
		str_ unique_file_name;
		str_ tmp_name;
		str_ path_and_unique_file_name;
		str_ extension = convertContentTypeToExtension(content_type);

		unique_file_name = getGMTYYYYMMDDHHMMSS();
		path_and_unique_file_name = file_path + unique_file_name + extension;
		if (access(path_and_unique_file_name.c_str(), F_OK) == NOT_FOUND)
		{
			return path_and_unique_file_name;
		}
		unique_file_name += "_";
		tmp_name = unique_file_name;
		for (unsigned int i = 0; i < UINT_MAX; i++)
		{
			unique_file_name = tmp_name;
			unique_file_name += utils::to_string(i);
			path_and_unique_file_name = file_path + unique_file_name + extension;
			if (access(path_and_unique_file_name.c_str(), F_OK) == NOT_FOUND)
			{
				return path_and_unique_file_name;
			}
		}
		return (path_and_unique_file_name);
	}

	static int writeFile(const str_ &file_content, const str_ &file_name)
	{
		std::ofstream file_to_put;

		file_to_put.open(file_name.c_str(), std::ios::out);
		if (!file_to_put.is_open() || !file_to_put)
		{
			return 500;
		}
		file_to_put << file_content; // << std::endl;
		file_to_put.close();
		return 200;
	}

#define FOUND 0

	int overwriteFile(const str_ &file_content, const str_ &file_name)
	{
		if (access(file_name.c_str(), F_OK) == FOUND &&
			access(file_name.c_str(), R_OK) == NOT_FOUND)
			return 403;
		return writeFile(file_content, file_name);
	}

	int makeAndPutFile(const str_ &file_content, const str_ &file_name)
	{
		if (access(file_name.c_str(), F_OK) == FOUND)
		{
			return 500;
		}
		return writeFile(file_content, file_name);
	}

	str_ discriminateExtension(const str_ &contents_path)
	{
		if (utils::contentEachExtension(contents_path, ".jpg"))
		{
			return "image/jpg";
		}
		else if (utils::contentEachExtension(contents_path, ".html"))
		{
			return "text/html";
		}
		else if (utils::contentEachExtension(contents_path, ".css"))
		{
			return "text/css";
		}
		return "text/plain";
	}

	str_ replaceAllFormStr(const str_ &raw_str, const str_ &str_to_be_replaced, const str_ &replacement_str)
	{
		str_ return_str = raw_str;

		for (size_t i = return_str.find(str_to_be_replaced); i != str_::npos; i = return_str.find(str_to_be_replaced))
		{
			return_str.replace(i, str_to_be_replaced.size(), replacement_str);
		}
		return return_str;
	}
}