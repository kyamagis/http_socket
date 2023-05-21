#include "../../includes/CGI.hpp"

CGI::CGI()
{
	;
}

CGI::CGI(const CGI &cgi)
{
	(void)cgi;
}

CGI &CGI::operator=(const CGI &rhs)
{
	(void)rhs;
	return *this;
}

CGI::CGI(str_ method, str_ cgi_path, map_env_ map_env, const str_ &file_path, const str_ &request_entity_body): 
			_method(method),
			_cgi_path(cgi_path),
			_map_env(map_env),
			_file_path(file_path),
			_request_entity_body(request_entity_body),
			_envp(NULL)
{
	;
}

void CGI::_convertMapToEnv(void)
{
	size_t size = this->_map_env.size();
	map_env_::iterator iter_begin = this->_map_env.begin();
	map_env_::iterator iter_end = this->_map_env.end();
	size_t i = 0;

	this->_envp = new char *[size + 1];
	for (map_env_::iterator iter = iter_begin; iter != iter_end; iter++)
	{
		this->_envp[i] = cgi_utils::convertStrToCharP(iter->first + "=" + iter->second);
		i++;
	}
	this->_envp[i] = NULL;
}

void CGI::_x_execve(char **argv)
{
	if (execve(argv[0], argv, this->_envp) == -1)
	{
		std::exit(EXIT_FAILURE);
	}
}

void CGI::_exeExecve()
{
	str_ command_line_options = this->_cgi_path + " " + this->_file_path;
	char **argv = cgi_utils::makeCharDoublePointer(command_line_options);
	if (_method == "POST")
	{
		utils::x_close(this->pipefd_for_send_request_entity_body_to_cgi[WRITE]);
		cgi_utils::x_dup2(this->pipefd_for_send_request_entity_body_to_cgi[READ], STDIN_FILENO);
		utils::x_close(this->pipefd_for_send_request_entity_body_to_cgi[READ]);
	}
	utils::x_close(this->pipefd_for_read_cgi_execution_result[READ]);
	cgi_utils::x_dup2(this->pipefd_for_read_cgi_execution_result[WRITE], STDOUT_FILENO);
	utils::x_close(this->pipefd_for_read_cgi_execution_result[WRITE]);
	CGI::_convertMapToEnv();
	CGI::_x_execve(argv);
}

#define READ_LEN 100 //適当な数字

int CGI::_readPipefdRead(int readfd)
{
	char buff[READ_LEN + 1];
	ssize_t len;

	while (true)
	{
		len = read(readfd, buff, READ_LEN);
		if (len == 0)
		{
			return 200;
		}
		if (len == -1)
		{
			std::cerr << "read fail" << std::endl;
			return 500;
		}
		buff[len] = '\0';
		this->_result += buff;
		if (len < READ_LEN)
		{
			return 200;
		}
	}
	return 0;
}

int CGI::_x_pipe()
{
	if (pipe(this->pipefd_for_read_cgi_execution_result) == -1)
	{
		return -1;
	}
	if (_method == "POST")
	{
		return 0;
	}
	if (pipe(this->pipefd_for_send_request_entity_body_to_cgi) == -1)
	{
		cgi_utils::closePutError(this->pipefd_for_read_cgi_execution_result[WRITE]);
		cgi_utils::closePutError(this->pipefd_for_read_cgi_execution_result[READ]);
		return -1;
	}
	return 0;
}

#define SUBSTR_LEN 100		   //適当な数字
#define WRITE_TIME_OUT 1000000 // 適当な数字

int CGI::_send_request_entity_body_to_cgi(pid_t pid)
{
	int fd = this->pipefd_for_send_request_entity_body_to_cgi[WRITE];
	size_t start_index = 0;
	str_ substr;
	str_ request_entity_body = this->_request_entity_body;
	clock_t start_time = cgi_utils::getMicroSec();

	while (true)
	{
		substr = request_entity_body.substr(start_index, SUBSTR_LEN);
		if (substr.size() < 1)
		{
			return 200;
		}
		if (write(fd, substr.c_str(), substr.size()) < 1)
		{
			// printf("CGI: entity body\n");
			return 500;
		}
		if (WRITE_TIME_OUT < cgi_utils::getMicroSec() - start_time)
		{
			// printf("kill\n");
			return cgi_utils::x_kill(pid);
		}
		if (substr.size() < SUBSTR_LEN)
		{
			return 200;
		}
		start_index += SUBSTR_LEN;
	}
	return 200;
}

int CGI::_caseOfPOSTmethod(pid_t pid)
{
	if (_method == "POST")
	{
		return 200;
	}
	cgi_utils::closePutError(this->pipefd_for_send_request_entity_body_to_cgi[READ]);
	int status_code = CGI::_send_request_entity_body_to_cgi(pid);
	cgi_utils::closePutError(this->pipefd_for_send_request_entity_body_to_cgi[WRITE]);
	return status_code;
}

int CGI::exeCGI()
{
	pid_t pid;

	if (CGI::_x_pipe() == -1)
	{
		return 500;
	}
	pid = fork();
	if (pid == -1)
	{
		return 500;
	}
	if (pid == 0)
	{
		CGI::_exeExecve();
	}
	cgi_utils::closePutError(this->pipefd_for_read_cgi_execution_result[WRITE]);
	int status_code = CGI::_caseOfPOSTmethod(pid);
	if (status_code == 200)
	{
		status_code = cgi_utils::waitpidAndKill(pid);
	}
	if (status_code == 200)
	{
		status_code = CGI::_readPipefdRead(this->pipefd_for_read_cgi_execution_result[READ]);
	}
	cgi_utils::closePutError(this->pipefd_for_read_cgi_execution_result[READ]);

	return status_code;
}

str_ CGI::getResult()
{
	return this->_result;
}
