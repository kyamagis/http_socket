#ifndef utils_hpp
# define utils_hpp



# include	<cerrno>
# include	<iostream>
# include	<fstream>
# include	<sstream>
# include	<vector>
# include	<map>
# include	<exception>
# include	<climits>
# include	<cstdlib>
# include	<unistd.h>
# include 	<strings.h>
# include	<signal.h>
# include	<fcntl.h>
# include	<sys/time.h>
# include	<sys/types.h>
# include	<sys/wait.h>
# define EMPTY_STRING ""
# define BRACKETS "{}"
# define SPACES "\t\n\v\f\r "
# define DIGITS "0123456789"

# define READ_TIME_OUT 10000000 // 適当な数字
# define CLOCK_FAIL UINT_MAX
# define debug(str) std::cout << str << std::endl

// # define debug(str) std::cout<<str<<std::endl


typedef	std::string			str_;
typedef	std::vector<str_>	vec_str_;
typedef	vec_str_::iterator	vec_str_iter_;
typedef	std::map<int, str_> map_status_ ;
typedef	std::map<int, str_> map_wday_ ;
typedef	std::map<int, str_> map_month_ ;

enum e_request_phase
{
	RECV_REQUEST,
	RECV_ENTITY_BODY,
	SEND_RESPONSE
};

namespace utils
{
	template <class T> str_ to_string(const T& target)
	{
		std::ostringstream oss;

		oss << target;
		return oss.str();
	}

	vec_str_	split_Str(const str_ &str, const str_ &delim);
	vec_str_	splitStr(const str_ &str, const str_ &delim);

	void		putError(str_ error_str);
	void		exitWithPutError(str_ error_str);
	void		exitWithError(const str_ &error_message);
	str_ 		intToStr(int num);
	void		x_close(int serv_socket, int line);
	void		x_close(int serv_socket);
	bool		contentEachExtension(const str_ &contents_path, const str_ &extention_look_for);
	int			x_fcntl(int fd, int cmd, int flg);
	str_		ft_strlwr(const str_ &str);
	long		myStrToLL(str_ str);
	str_		joinPath(const str_& path_front, const str_& path_back);
	clock_t 	getMicroSec(clock_t time_limit);
}


namespace parse_utils
{
	void	deleteSemicolomn(vec_str_& split_str);

	class InvalidSyntaxException : public std::exception
	{
		virtual const char	*what() const throw();
	};

	class OverflowException : public std::exception
	{
		virtual const char	*what() const throw();
	};

	template<typename T>
	T	strToDigitOverflow(const std::string str, T min, T max)
	{
		char	*remain = NULL;
		double	value;

		value = std::strtod(str.c_str(), &remain);
		if (!remain || remain[0])
			throw InvalidSyntaxException();
		if (value < static_cast<double>(min) || static_cast<double>(max) < value)
			throw OverflowException();
		return (static_cast<T>(value));
	}
}

#endif
