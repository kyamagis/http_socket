#ifndef CGI_HPP
# define CGI_HPP

# include	<iostream>
# include	<map>
# include	<unistd.h>
# include	<signal.h>
# include	"./utils.hpp"
# include	"./Value.hpp"
# include	"./Request.hpp"
# include	"./cgi_utils.hpp"

# define READ 0
# define WRITE 1

typedef std::map<std::string, std::string>	map_env_;

enum e_cgi_phase
{
	CGI_start,
	CGI_write,
	CGI_read_header,
	CGI_read_body,
	CGI_end
};

class CGI
{
	private:
		CGI(const CGI &cgi);
		CGI &operator=(const CGI &rhs);

		str_		_method;
		str_		_cgi_path;
		map_env_	_map_env;
		str_		_file_path;
		str_		_request_entity_body;
		char		**_envp;

		pid_t	_pid;
		int		_pipefd_for_read_cgi_execution_result[2];
		int		_pipefd_for_send_request_entity_body_to_cgi[2];
		str_	_cgi_exec_result;
		clock_t _time_limit;	

		void	_x_execve(char **argv);
		int		_pipeAndFcntl();
		
		ssize_t	_readExecResulet();
		
		void	_exeExecve();
		void	_convertMapToEnv(void);
		int		_parseMime(const str_ &lower_str, const vec_str_ &vec_split_a_header);
		int		_parseContentType(const vec_str_ &vec_split_a_header);
		int		_parseContentLength(const vec_str_ &vec_split_header);
		int		_parseCGIResponseHeaders();
		int		_parseCGIResponseEntityBody();
		int		_forkAndClose();

	public:

		CGI();
		void	setCGI(str_ method, str_ cgi_path, map_env_ map_env, const str_ &file_path
			, const str_& request_entity_body);

		enum e_cgi_phase		cgi_phase;
		RValue<str_>			content_type;
		RValue<unsigned int>	content_length;

		int		startCGI();
		int		readAndWaitpid();
		str_	getCGIExecResult();
		int		writeRequestEntityBodyToCGI();
		int		getWriteFd();
		int		getReadFd();

};

#endif
