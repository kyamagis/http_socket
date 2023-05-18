
#include "./utils.hpp"

#define vec_str_ std::vector<str_>

namespace utils
{
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
}