
#include "../includes/utils.hpp"

#include <string.h>
#include <fcntl.h>
#include <ctime>

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

	bool contentEachExtension(const str_ &contents_path, const str_ &extention_look_for)
	{
		if (contents_path.find(extention_look_for) == std::string::npos)
		{
			return false;
		}
		size_t extension_len = extention_look_for.size();
		if (contents_path.size() - contents_path.rfind(extention_look_for) != extension_len)
		{
			return false;
		}
		return true;
	}

	int	x_fcntl(int fd, int cmd, int flg)
	{
		if (fcntl(fd, cmd, flg) == -1)
		{
			utils::putError("fcntl() failed");
			utils::x_close(fd);
			return -1;
		}
		return 0;
	}

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

	str_ joinPath(const str_ &path_front, const str_ &path_back)
	{
		str_ full_path = path_front + path_back;

		size_t pos_double_slash = full_path.find("//");
		if (pos_double_slash != str_::npos && pos_double_slash == path_front.length() - 1)
			full_path.erase(pos_double_slash, 1);
		return (full_path);
	}

	clock_t getMicroSec(clock_t time_limit)
	{
		clock_t t = std::clock(); // 納得していない 定数時間を測るのに適しているとは思えない

		if (t == CLOCK_FAIL)
		{
			return CLOCK_FAIL;
		}
		return (t + time_limit);
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
