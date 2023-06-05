
#include "../../includes/method_utils.hpp"

#include <climits>
#include <ctime>
#include <fstream>


namespace method_utils
{
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
			return 404;
		}
		file_to_put << file_content; // << std::endl;
		file_to_put.close();
		return 200;
	}

#define FOUND 0

	int overwriteFile(const str_ &file_content, const str_ &file_name)
	{
		if (access(file_name.c_str(), F_OK) == NOT_FOUND)
		{
			return 404;
		}
		if (access(file_name.c_str(), F_OK) == FOUND &&
			access(file_name.c_str(), R_OK) == NOT_FOUND)
			return 403;
		return writeFile(file_content, file_name);
	}

	int makeAndPutFile(const str_ &file_content, const str_ &file_name)
	{
		if (access(file_name.c_str(), F_OK) == FOUND)
		{
			return 403;
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
