#ifndef CGI_HPP
# define CGI_HPP

# include	<iostream>
# include	<map>
# include	<unistd.h>
# include	<signal.h>
# include	"./utils.hpp"
# include	"./Value.hpp"
# include	"./cgi_utils.hpp"

# define READ 0
# define WRITE 1

typedef std::map<std::string, std::string>	map_env_;

class CGI
{
	private:
		CGI();
		CGI(const CGI &cgi);
		CGI &operator=(const CGI &rhs);

		str_		_result;
		map_env_	_map_env;
		str_		_file_path;
		char		**_envp;
		const str_	_cgi_path;
		Value<str_>	_request_entity_body;
		int			pipefd_for_read_cgi_execution_result[2];
		int			pipefd_for_send_request_entity_body_to_cgi[2];

		void	_x_execve(char **argv);
		int		_x_pipe();
		int		_send_request_entity_body_to_cgi(pid_t pid);
		int		_caseOfPOSTmethod(pid_t pid);
		int		_readPipefdRead(int readfd);
		void	_exeExecve();
		void	_convertMapToEnv(void);

	public:

		CGI(str_ cgi_path, map_env_ map_env, const str_ &file_path
			, const Value<str_>& request_entity_body);

		CGI(str_ cgi_path_arg, map_env_	map_env, const Value<str_>& file_path, //消す
			const Value<str_>& request_entity_body);

		int		exeCGI();
		str_	getResult();
};

#endif
