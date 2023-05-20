
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
