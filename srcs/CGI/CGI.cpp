#include "../../includes/CGI.hpp"

CGI::CGI(): cgi_phase(CGI_start)
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

CGI::~CGI()
{

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
	utils::putError(argv[0]);
	utils::putError(argv[1]);
	if (execve(argv[0], argv, this->_envp) == -1)
	{
		utils::putError("execve() fail");
		std::exit(EXIT_FAILURE);
	}
}

void	CGI::_exeExecve()
{
	str_ command_line_options = this->_cgi_path + " " + this->_file_path;
	char **argv = cgi_utils::makeCharDoublePointer(command_line_options);
	if (_method == "POST")
	{
		utils::x_close(this->_pipefd_for_send_request_entity_body_to_cgi[WRITE]);
		cgi_utils::x_dup2(this->_pipefd_for_send_request_entity_body_to_cgi[READ], STDIN_FILENO);
		utils::x_close(this->_pipefd_for_send_request_entity_body_to_cgi[READ]);
	}
	utils::x_close(this->_pipefd_for_read_cgi_execution_result[READ]);
	cgi_utils::x_dup2(this->_pipefd_for_read_cgi_execution_result[WRITE], STDOUT_FILENO);
	utils::x_close(this->_pipefd_for_read_cgi_execution_result[WRITE]);
	CGI::_convertMapToEnv();
	CGI::_x_execve(argv);
}

int	CGI::_parseMime(const str_ &lower_str, const vec_str_ &vec_split_a_header)
{
	size_t size = lower_str.size();
	size_t i = 0;
	size_t slash_count = 0;
	str_ type_name;

	if (size < 3 || lower_str[0] == '/')
	{
		return 500;
	}
	for (; i < size && lower_str[i] != ';'; i++)
	{
		if (lower_str[i] == '/')
		{
			slash_count++;
		}
	}
	if (slash_count != 1 || lower_str[i - 1] == '/')
	{
		return 500;
	}
	if (lower_str[i] != ';' && vec_split_a_header.size() != 2)
	{
		return 500;
	}
	type_name = lower_str.substr(0, i);
	if (type_name != "image/jpg" && type_name != "text/html" && type_name != "text/css")
	{
		this->content_type.setValue((str_) "text/plain");
	}
	else
	{
		this->content_type.setValue(type_name);
	}
	return 200;
}

int	CGI::_parseContentType(const vec_str_ &vec_split_a_header)
{
	if (this->content_type.getStatus() != NOT_SET)
	{
		return 500;
	}
	str_ lower_str = utils::ft_strlwr(vec_split_a_header[1]);
	if (CGI::_parseMime(lower_str, vec_split_a_header) == 500)
	{
		return 500;
	}
	return 200;
}

int	CGI::_parseContentLength(const vec_str_ &vec_split_header)
{
	if (this->content_length.getStatus() != NOT_SET)
	{
		return 500;
	}
	if (vec_split_header.size() != 2)
	{
		return 500;
	}
	long value = utils::myStrToLL(vec_split_header[1]);
	if (value < 0)
	{
		return 500;
	}
	this->content_length.setValue((unsigned int)value);
	return 200;
}

int	CGI::_parseCGIResponseHeaders()
{
	size_t	cr_lf_pos = this->_cgi_exec_result.find("\r\n\r\n");

	if (cr_lf_pos == str_::npos)
	{
		return CONTINUE;
	}
	// レスポンスヘッダーが終了したことを検知したときの挙動
	str_	cgi_response_header = this->_cgi_exec_result.substr(0, cr_lf_pos);
	this->_cgi_exec_result = this->_cgi_exec_result.substr(cr_lf_pos + 4);

	vec_str_	vec_cgi_response_headers = utils::split_Str(cgi_response_header, "\r\n");
	vec_str_	vec_split_a_header;
	str_		lower_str;

	for (size_t i = 0; i < vec_cgi_response_headers.size(); i++)
	{
		vec_split_a_header = utils::split_Str(vec_cgi_response_headers[i], ": ");

		lower_str = utils::ft_strlwr(vec_split_a_header[0]);
		if (lower_str == "content-type" && 1 < vec_split_a_header.size())
		{
			return CGI::_parseContentType(vec_split_a_header);
		}
		else if (lower_str == "content-length" && 1 < vec_split_a_header.size())
		{
			return CGI::_parseContentLength(vec_split_a_header);
		}
	}
	if (this->content_type.getStatus() == NOT_SET || this->content_length.getStatus() == NOT_SET)
	{
		debug("type, length not set");
		return 500;
	}
	return 200;
}

#define READ_LEN 1024 //適当な数字

ssize_t	CGI::_readExecResulet()
{
	char	buff[READ_LEN + 1];
	int		readfd = this->_pipefd_for_read_cgi_execution_result[READ];

	ssize_t	read_len = read(readfd, buff, READ_LEN);
	if (read_len == -1)
	{
		if (errno != EWOULDBLOCK)
		{
			utils::putError("read() fail");
			return -1;
		}
		return 0;
	}
	if (0 < read_len)
	{
		buff[read_len] = '\0';
		this->_cgi_exec_result += buff;
	}
	return read_len;
}

int	CGI::_parseCGIResponseEntityBody()
{
	if (this->content_length.getValue() <= this->_cgi_exec_result.size())
	{
		this->_cgi_exec_result.substr(0, this->content_length.getValue());
		return END;
	}
	return CONTINUE;
}

int CGI::readAndWaitpid()
{
	int		readfd = this->_pipefd_for_read_cgi_execution_result[READ];
	clock_t	start_time = cgi_utils::getMicroSec(0);
	int		status_code = 0;

	if (CGI::_readExecResulet() == -1)
	{
		debug("read_len == -1");
		status_code = 500;
	}
	else if (this->cgi_phase == CGI_read_header)
	{
		status_code = CGI::_parseCGIResponseHeaders();
		if (status_code == 200)
		{
			this->cgi_phase = CGI_read_body;
		}
	}
	else if (this->cgi_phase == CGI_read_body)
	{
		status_code = CGI::_parseCGIResponseEntityBody();
		if (status_code == END)
		{
			this->cgi_phase = CGI_end;
		}
	}
	clock_t	end_time = cgi_utils::getMicroSec(0);
	if (this->_time_limit < end_time - start_time)
	{
		status_code = 500;
	}
	else
	{
		this->_time_limit += - end_time + start_time;
	}
	int	wstatus;
	if (status_code == END || status_code == 500)
	{
		utils::x_close(readfd);
		if (waitpid(this->_pid, &wstatus, WNOHANG) == 0)
		{
			cgi_utils::x_kill(this->_pid);
		}
		return status_code;
	}
	if (waitpid(this->_pid, &wstatus, WNOHANG) == -1)
	{
		if (!WIFEXITED(wstatus))
		{
			return 500;
		}
		switch (WEXITSTATUS(wstatus))
		{
			case EXIT_SUCCESS:
				return CONTINUE;
			default:
				return 500;
		}
	}
	return CONTINUE;
}

int CGI::_pipeAndFcntl()
{
	if (pipe(this->_pipefd_for_read_cgi_execution_result) == -1)
	{
		return -1;
	}
	if (utils::x_fcntl(this->_pipefd_for_read_cgi_execution_result[READ], F_SETFL, O_NONBLOCK) == -1)
	{
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[WRITE]);
		return -1;
	}
	if (_method != "POST")
	{
		return 0;
	}
	if (pipe(this->_pipefd_for_send_request_entity_body_to_cgi) == -1)
	{
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[WRITE]);
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[READ]);
		return -1;
	}
	if (utils::x_fcntl(this->_pipefd_for_read_cgi_execution_result[READ], F_SETFL, O_NONBLOCK) == -1)
	{
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[WRITE]);
		return -1;
	}
	return 0;
}

#define TIME_LIMIT 1000000 // 適当な数字

int CGI::writeRequestEntityBodyToCGI()
{
	int	writefd = this->_pipefd_for_send_request_entity_body_to_cgi[WRITE];

	clock_t	start_time = cgi_utils::getMicroSec(0);
	if (this->_request_entity_body.size() == 0)
	{
		utils::x_close(writefd);
		this->cgi_phase = CGI_read_header;
		return END;
	}
	ssize_t	wrote_len = write(writefd, this->_request_entity_body.c_str(), this->_request_entity_body.size());
	clock_t	end_time = cgi_utils::getMicroSec(0);
	if (wrote_len < 1 || this->_time_limit < end_time - start_time)
	{
		utils::x_close(writefd);
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[READ]);
		cgi_utils::x_kill(this->_pid);
		return 500;
	}
	this->_time_limit += - end_time + start_time;
	this->_request_entity_body = this->_request_entity_body.substr(wrote_len);
	if (this->_request_entity_body.size() == 0)
	{
		utils::x_close(writefd);
		this->cgi_phase = CGI_read_header;
		return END;
	}
	return CONTINUE;
}

int	CGI::_forkAndClose()
{
	this->_pid = fork();
	if (this->_pid == -1)
	{
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[WRITE]);
		utils::x_close(this->_pipefd_for_read_cgi_execution_result[READ]);
		if (this->_method == "POST")
		{
			utils::x_close(this->_pipefd_for_send_request_entity_body_to_cgi[WRITE]);
			utils::x_close(this->_pipefd_for_send_request_entity_body_to_cgi[READ]);
		}
		return -1;
	}
	return 0;
}

int	CGI::startCGI()
{
	if (CGI::_pipeAndFcntl() == -1)
	{
		return 500;
	}
	if (CGI::_forkAndClose() == -1)
	{
		return 500;
	}
	if (this->_pid == 0)
	{
		CGI::_exeExecve();
	}
	utils::x_close(this->_pipefd_for_read_cgi_execution_result[WRITE]);
	if (this->_method == "GET")
	{
		this->cgi_phase = CGI_read_header;
	}
	else if (_method == "POST")
	{
		utils::x_close(this->_pipefd_for_send_request_entity_body_to_cgi[READ]);
		this->cgi_phase = CGI_write;
	}
	return this->cgi_phase;
}

str_	CGI::getCGIExecResult()
{
	return this->_cgi_exec_result;
}


void	CGI::setCGI(str_ method, str_ cgi_path, map_env_ map_env, 
				const str_ &file_path, const str_ &request_entity_body)
{
	this->_method = method;
	this->_cgi_path = cgi_path;
	this->_map_env = map_env;
	this->_file_path = file_path;
	this->_request_entity_body = request_entity_body;
	this->_envp = NULL;
	this->_time_limit = TIME_LIMIT;
}

int		CGI::getWriteFd()
{
	return this->_pipefd_for_send_request_entity_body_to_cgi[WRITE];
}

int		CGI::getReadFd()
{
	return this->_pipefd_for_read_cgi_execution_result[READ];
}



	// char str[50 + 1];
	// ssize_t len = read(STDIN_FILENO, str, 50);
	// str[len] = '\0';
	// debug("******************");
	// debug(str);
	// debug("******************");

/* int main(void)
{
	debug("GET CGI");
	{
		map_env_	map_env;

		map_env["METHOD"] = "GET";
		map_env["URI"] = "/";
		map_env["VERSION"] = "HTTP/1.1";
		map_env["HOST"] = "localhost";
		map_env["CONTENT_TYPE"] = "text/html";
		map_env["CONTENT_LENGTH"] = "30";


		CGI	cgi;
		str_ method = "GET";
		str_ cgi_path = "/bin/bash";
		str_ file_name = "contents/cgi_files/GET.sh";
		str_ body = "dosukoi";

		cgi.setCGI(method, cgi_path, map_env, file_name, body);
		if (cgi.startCGI() == 500)
		{
			debug("start() faile");
			return 1;
		}
		while(true)
		{
			int status = cgi.readAndWaitpid();
			if (status == 500)
			{
				debug("readAndWaitpid() faile");
				return 1;
			}
			if (status == 200 || status == CONTINUE)
			{
				//debug("continue");
				continue;
			}
			if (status == END)
			{
				debug(cgi.getCGIExecResult());
				return 0;
			}
		}
	}
	debug("Post CGI");
	{
		map_env_	map_env;

		map_env["METHOD"] = "POST";
		map_env["URI"] = "/";
		map_env["VERSION"] = "HTTP/1.1";
		map_env["HOST"] = "localhost";
		map_env["CONTENT_TYPE"] = "text/html";
		map_env["CONTENT_LENGTH"] = "30";


		CGI	cgi;
		str_ method = "POST";
		str_ cgi_path = "/bin/bash";
		str_ file_name = "contents/cgi_files/POST.sh";
		str_ body = "dosukoi";

		cgi.setCGI(method, cgi_path, map_env, file_name, body);
		if (cgi.startCGI() == 500)
		{
			debug("start() faile");
			return 1;
		}
		int status;
		while(true)
		{
			if (cgi.cgi_phase == CGI_write)
			{
				status = cgi.writeRequestEntityBodyToCGI();
				if (status == 500)
				{
					debug("writeRequestEntityBodyToCGI() faile");
					return 1;
				}
			}
			else
			{
				status = cgi.readAndWaitpid();
				if (status == 500)
				{
					debug("readAndWaitpid() faile");
					return 1;
				}
				if (status == 200 || status == CONTINUE)
				{
					//debug("continue");
					continue;
				}
				if (status == END)
				{
					debug(cgi.getCGIExecResult());
					return 0;
				}
			}
			
		}
	}
} */