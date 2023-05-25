#include "../../includes/IOMultiplexing.hpp"

#define BUFF_SIZE 10240
#define DEQ_RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].deq_response_message
#define RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].deq_response_message[0].response_message
#define MAKE_RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].makeResponseMessage(response_message)
#define METHOD_P this->_fd_MessageManagement[accepted_socket].method_p
#define REQUEST_MESSAGE this->_fd_MessageManagement[accepted_socket].request_message
#define PARSE_REQUEST_MESSAGE this->_fd_MessageManagement[accepted_socket].parseRequstMessage()
#define INIT_REQUEST_CLASS this->_fd_MessageManagement[accepted_socket].initResponseClass()
#define ATTRIBUTION this->_fd_MessageManagement[fd].attribution

#define WRITE_FD this->_fd_MessageManagement[accepted_socket].method_p->cgi.getWriteFd()
#define READ_FD this->_fd_MessageManagement[accepted_socket].method_p->cgi.getReadFd()



IOMultiplexing::IOMultiplexing()
{

}

IOMultiplexing::IOMultiplexing(const IOMultiplexing &io)
{
	(void)io;
}

IOMultiplexing &IOMultiplexing::operator=(const IOMultiplexing &rhs)
{
	(void)rhs;
	return *this;
}

IOMultiplexing::IOMultiplexing(const vec_sever_	&servers, const vec_int_ &vec_ports): _servers(servers)
{
	IOMultiplexing::createVecListeningSocket(vec_ports);
	IOMultiplexing::initMasterReadfds();
	_timeout.tv_sec = 0;
	_timeout.tv_usec = 200;
	FD_ZERO(&this->_master_writefds);
}

IOMultiplexing::~IOMultiplexing()
{

}

///////////////////////////////////////////////////////////////////////////////


void	IOMultiplexing::createVecListeningSocket(const vec_int_ &vec_ports)
{
	int	listening_socket;
	int	max_descripotor = 0;

	for (size_t i = 0; i < vec_ports.size(); i++)
	{
		if (max_descripotor == FD_SETSIZE - 2)
			return ;
		listening_socket = IOM_utils::createListeningSocket(vec_ports[i]);
		std::cout << "port: " << vec_ports[i];
		if (listening_socket == -1)
		{
			std::cout << ", bind() failed: " << strerror(errno) << std::endl;
		}
		else if (listening_socket != -1)
		{
			this->_vec_listening_socket.push_back(listening_socket);
			std::cout << ", fd: " << listening_socket << std::endl;
		}
		max_descripotor = listening_socket;
	}
}

void IOMultiplexing::initMasterReadfds()
{
	this->_max_descripotor = 0;

	FD_ZERO(&this->_master_readfds);
	for (size_t i = 0; i < _vec_listening_socket.size(); i++)
	{
		FD_SET(_vec_listening_socket[i], &this->_master_readfds);
		if (this->_max_descripotor < _vec_listening_socket[i])
			this->_max_descripotor = _vec_listening_socket[i];
	}
}

void	IOMultiplexing::_decrementMaxDescripotor(int fd)
{
	if (fd == this->_max_descripotor)
	{	
		while (!FD_ISSET(this->_max_descripotor, &this->_master_readfds) && 
			!FD_ISSET(this->_max_descripotor, &this->_master_writefds))
			this->_max_descripotor -= 1;
	}
}

void	IOMultiplexing::sendResponse(int accepted_socket)
{
	if (DEQ_RESPONSE_MESSAGE.size() == 0)
		return ;
	ssize_t	sent_len = send(accepted_socket, RESPONSE_MESSAGE.c_str(), RESPONSE_MESSAGE.size(), MSG_DONTWAIT);

	if (sent_len == -1)
	{
		debug("recv == -1");
	}
	if (sent_len == -1 && errno != EWOULDBLOCK)
	{
		utils::exitWithPutError("send() failed");
	}
	if (sent_len < 1 || RESPONSE_MESSAGE.size() == (size_t)sent_len)
	{
		FD_CLR(accepted_socket, &this->_master_writefds);
		IOMultiplexing::_decrementMaxDescripotor(accepted_socket);
		utils::x_close(accepted_socket);

		debug("---------------------------------------------");
		debug(accepted_socket);
		debug(RESPONSE_MESSAGE);
		debug("---------------------------------------------");

		this->_fd_MessageManagement.erase(accepted_socket);
		
	}
	else if ((size_t)sent_len < RESPONSE_MESSAGE.size())
	{
		RESPONSE_MESSAGE = RESPONSE_MESSAGE.substr(0, sent_len);
	}
}

bool	IOMultiplexing::containsListeningSocket(int fd)
{
	for (size_t i = 0; i < this->_vec_listening_socket.size(); i++)
		if (this->_vec_listening_socket[i] == fd)
			return true;
	return false;
}

void	IOMultiplexing::createAcceptedSocket(int listening_socket)
{
	int	accepted_socket;

	while(true)
	{
		if (this->_max_descripotor == FD_SETSIZE - 1)
			return ;
		accepted_socket = accept(listening_socket, NULL, NULL);
		if (accepted_socket == -1)
		{
			if (errno != EWOULDBLOCK)
			{
				utils::exitWithPutError("accept() failed");
			}
			debug("accept() == -1");
			return ;
		}
		this->_fd_MessageManagement.insert(std::pair<int, MessageManagement>(accepted_socket, MessageManagement()));
		this->_fd_MessageManagement[accepted_socket].MessageManagement::searchServer(accepted_socket, this->_servers);
		FD_SET(accepted_socket, &this->_master_readfds);
		if (this->_max_descripotor < accepted_socket)
			this->_max_descripotor = accepted_socket;
	}
}

void	IOMultiplexing::_setSendResponse(int accepted_socket, const t_response_message &response_message)
{
	int	fd = accepted_socket;

	DEQ_RESPONSE_MESSAGE.push_back(response_message);
	ATTRIBUTION = NOT_CGI;
	INIT_REQUEST_CLASS;
	delete METHOD_P;
	METHOD_P = NULL;
}

void	IOMultiplexing::_setStoreCGIResponse(int accepted_socket)
{
	int	fd = accepted_socket;

	FD_SET(READ_FD, &this->_master_readfds);
	ATTRIBUTION = READ_CGI;
	this->_pipefd_fd[READ_FD] = accepted_socket;
	if (this->_max_descripotor < READ_FD)
		this->_max_descripotor = READ_FD;
}

void	IOMultiplexing::_setWriteCGI(int accepted_socket)
{
	int	fd = accepted_socket;

	FD_SET(WRITE_FD, &this->_master_writefds);
	ATTRIBUTION = WRITE_CGI;
	this->_pipefd_fd[WRITE_FD] = accepted_socket;
	if (this->_max_descripotor < WRITE_FD)
		this->_max_descripotor = WRITE_FD;
}

/* 基底回数以上になったら、閉じる動作を入れる */
void	IOMultiplexing::storeRequestToMap(int fd)
{
	char	buffer[BUFF_SIZE + 1];
	int		accepted_socket = fd; // わかりやすくするために代入した.技術的な意味はない

	if (!IOM_utils::recvRequest(accepted_socket, buffer))
		return ;

	REQUEST_MESSAGE += buffer;
	if (PARSE_REQUEST_MESSAGE == CONTINUE)
	{
		return ;
	}

	t_response_message	response_message;

	FD_CLR(accepted_socket, &this->_master_readfds);
	FD_SET(accepted_socket, &this->_master_writefds);
	int	cgi_flg = MAKE_RESPONSE_MESSAGE;
	debug(this->_fd_MessageManagement[accepted_socket]);
	if (cgi_flg == CGI_write)
	{
		IOMultiplexing::_setWriteCGI(accepted_socket);
	}
	else if (cgi_flg == CGI_read_header)
	{
		IOMultiplexing::_setStoreCGIResponse(accepted_socket);
	}
	else
	{
		IOMultiplexing::_setSendResponse(accepted_socket, response_message);
	}
}

bool	IOMultiplexing::isCGIWriteFd(int write_fd)
{
	map_pipefd_fd_ite_	ite = this->_pipefd_fd.find(write_fd);

	if (ite == this->_pipefd_fd.end())
	{
		return false;
	}
	if (this->_fd_MessageManagement[ite->second].attribution != WRITE_CGI)
	{
		return false;
	}
	return true;
}

bool	IOMultiplexing::isCGIReadFd(int read_fd)
{
	map_pipefd_fd_ite_	ite = this->_pipefd_fd.find(read_fd);

	if (ite == this->_pipefd_fd.end())
	{
		return false;
	}
	if (this->_fd_MessageManagement[ite->second].attribution != READ_CGI)
	{
		return false;
	}
	return true;
}

void	IOMultiplexing::storeCGIResponse(int read_fd)
{
	int	accepted_socket = this->_pipefd_fd[read_fd];
	t_response_message	response_message;
	
	if (this->_fd_MessageManagement[accepted_socket].readCGIResponse(response_message) == CONTINUE)
	{
		return ;
	}
	// CGI の関数でread_fdはclose()した.
	FD_CLR(read_fd, &this->_master_readfds);
	this->_pipefd_fd.erase(READ_FD);
	IOMultiplexing::_decrementMaxDescripotor(read_fd);
	IOMultiplexing::_setSendResponse(accepted_socket, response_message);
}

void	IOMultiplexing::writeCGI(int write_fd) //　エラーの場合、レスポンスメッセージを書く
{
	int	accepted_socket = this->_pipefd_fd[write_fd];
	t_response_message	response_message;
	int	status = this->_fd_MessageManagement[accepted_socket].writeCGIRequest(response_message);

	if (status == CONTINUE)
	{
		return ;
	}
	FD_CLR(write_fd, &this->_master_writefds);
	this->_pipefd_fd.erase(WRITE_FD);
	IOMultiplexing::_decrementMaxDescripotor(write_fd);
	if (status == 500)
	{
		IOMultiplexing::_setSendResponse(accepted_socket, response_message);
	}
	else if (status == END)
	{
		IOMultiplexing::_setStoreCGIResponse(accepted_socket);
	}
}


void	IOMultiplexing::IOMultiplexingLoop()
{
	int	ready;

	while(true)
	{
		std::memcpy(&this->_writefds, &this->_master_writefds, sizeof(this->_master_writefds));
		std::memcpy(&this->_readfds, &this->_master_readfds, sizeof(_master_readfds));
		ready = select(this->_max_descripotor + 1, &this->_readfds, &this->_writefds, NULL, &this->_timeout);
		if (ready == 0)
		{
			continue ;
		}
		else if (ready == -1)
			utils::exitWithPutError("select() failed");
		else
		{
			for (int fd = 0; fd < this->_max_descripotor + 1; fd++)
			{
				if (FD_ISSET(fd, &this->_writefds))
				{
					if (IOMultiplexing::isCGIWriteFd(fd))
					{	
						IOMultiplexing::writeCGI(fd);
					}
					else if (ATTRIBUTION == NOT_CGI)
					{
						IOMultiplexing::sendResponse(fd);
						std::cout << "clnt_socket: " << fd << ", max_descripotor: " 
								<< this->_max_descripotor << std::endl;
					}
				}
				else if (FD_ISSET(fd, &this->_readfds))
				{
					if (IOMultiplexing::containsListeningSocket(fd))
					{
						IOMultiplexing::createAcceptedSocket(fd);
					}
					else if (IOMultiplexing::isCGIReadFd(fd))
					{
						IOMultiplexing::storeCGIResponse(fd);
					}
					else if (ATTRIBUTION == NOT_CGI)
					{
						IOMultiplexing::storeRequestToMap(fd);
					}
				}
			}
		}
	}
}

