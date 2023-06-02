#include "../../includes/IOMultiplexing.hpp"

#define BUFF_SIZE 10240
#define DEQ_RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].deq_response_message
#define RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].deq_response_message[0].response_message
#define MAKE_RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].makeResponseMessage(response_message, this->_max_descripotor)
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

IOMultiplexing::IOMultiplexing(const vec_sever_	&servers): _servers(servers)
{
	IOMultiplexing::_createVecListeningSocket();
	IOMultiplexing::_initMasterReadfds();
	FD_ZERO(&this->_master_writefds);
}

IOMultiplexing::~IOMultiplexing()
{

}

///////////////////////////////////////////////////////////////////////////////


void	IOMultiplexing::_createVecListeningSocket()
{
	int	listening_socket;
	int	max_descripotor = 0;

	for (size_t i = 0; i < this->_servers.size(); i++)
	{
		if (max_descripotor == FD_SETSIZE - 2)
			return ;
		listening_socket = IOM_utils::createListeningSocket(this->_servers[i].listen_host.getValue().c_str(), 
															this->_servers[i].listen_port.getValue());
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

void IOMultiplexing::_initMasterReadfds()
{
	this->_max_descripotor = 0;

	FD_ZERO(&this->_master_readfds);
	for (size_t i = 0; i < this->_vec_listening_socket.size(); i++)
	{
		FD_SET(this->_vec_listening_socket[i], &this->_master_readfds);
		if (this->_max_descripotor < this->_vec_listening_socket[i])
			this->_max_descripotor = this->_vec_listening_socket[i];
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

void	IOMultiplexing::_eraseMMAndCloseFd(int accepted_socket, fd_set *fds)
{
	FD_CLR(accepted_socket, fds);
	IOMultiplexing::_decrementMaxDescripotor(accepted_socket);
	utils::x_close(accepted_socket);
	if (METHOD_P)
	{
		delete METHOD_P;
		METHOD_P = NULL;
	}
	this->_fd_MessageManagement.erase(accepted_socket);
}

void	IOMultiplexing::_switchToRecvRequest(int accepted_socket)
{
	FD_CLR(accepted_socket, &this->_master_writefds);
	FD_SET(accepted_socket, &this->_master_readfds);
	if (METHOD_P)
	{
		delete METHOD_P;
		METHOD_P = NULL;
	}
	this->_fd_MessageManagement.erase(accepted_socket);
	this->_fd_MessageManagement.insert(std::pair<int, MessageManagement>(accepted_socket, MessageManagement()));
}

void	IOMultiplexing::_switchToSendResponse(int accepted_socket, const t_response_message &response_message)
{
	int	fd = accepted_socket;

	DEQ_RESPONSE_MESSAGE.push_back(response_message);
	ATTRIBUTION = NOT_CGI;
	INIT_REQUEST_CLASS;
}

void	IOMultiplexing::_switchToReadCGIResponse(int accepted_socket)
{
	int	fd = accepted_socket;

	FD_SET(READ_FD, &this->_master_readfds);
	ATTRIBUTION = READ_CGI;
	this->_pipefd_fd[READ_FD] = accepted_socket;
	if (this->_max_descripotor < READ_FD)
		this->_max_descripotor = READ_FD;
}

void	IOMultiplexing::_switchToWriteCGI(int accepted_socket)
{
	int	fd = accepted_socket;

	FD_SET(WRITE_FD, &this->_master_writefds);
	ATTRIBUTION = WRITE_CGI;
	this->_pipefd_fd[WRITE_FD] = accepted_socket;
	if (this->_max_descripotor < WRITE_FD)
		this->_max_descripotor = WRITE_FD;
}

void	IOMultiplexing::_sendResponse(int accepted_socket)
{
	if (DEQ_RESPONSE_MESSAGE.size() == 0)
		return ;
	ssize_t	sent_len = send(accepted_socket, RESPONSE_MESSAGE.c_str(), RESPONSE_MESSAGE.size(), MSG_DONTWAIT);

	if (sent_len < 1)
	{
		debug("client closed socket");
		IOMultiplexing::_eraseMMAndCloseFd(accepted_socket, &this->_master_writefds);
	}
	else if (RESPONSE_MESSAGE.size() == (size_t)sent_len)
	{
		std::cout << "---------------------------------------------" << std::endl
		<< "accepted_socket = " << accepted_socket << std::endl
		<< RESPONSE_MESSAGE 
		<< "---------------------------------------------" << std::endl;

		if (DEQ_RESPONSE_MESSAGE[0].connection_flg == CONNECTION_CLOSE)
		{
			IOMultiplexing::_eraseMMAndCloseFd(accepted_socket, &this->_master_writefds);
		}
		else if (DEQ_RESPONSE_MESSAGE[0].connection_flg == CONNECTION_KEEP_ALIVE)
		{
			IOMultiplexing::_switchToRecvRequest(accepted_socket);
			
		}
	}
	else if ((size_t)sent_len < RESPONSE_MESSAGE.size())
	{
		RESPONSE_MESSAGE = RESPONSE_MESSAGE.substr(sent_len);
		
	}

}

bool	IOMultiplexing::_containsListeningSocket(int fd)
{
	for (size_t i = 0; i < this->_vec_listening_socket.size(); i++)
		if (this->_vec_listening_socket[i] == fd)
			return true;
	return false;
}

void	IOMultiplexing::_createAcceptedSocket(int listening_socket)
{
	int	accepted_socket;

	while(true)
	{
		if (this->_max_descripotor == FD_SETSIZE - 1)
			return ;
		accepted_socket = accept(listening_socket, NULL, NULL);
		if (accepted_socket == -1)
		{
			/* if (errno != EWOULDBLOCK)
			{
				utils::exitWithPutError("accept() failed");
			} */
			debug("accept() == -1");
			return ;
		}
		this->_fd_MessageManagement.insert(std::pair<int, MessageManagement>(accepted_socket, MessageManagement()));
		FD_SET(accepted_socket, &this->_master_readfds);
		if (this->_max_descripotor < accepted_socket)
			this->_max_descripotor = accepted_socket;
	}
}


/* 基底回数以上になったら、閉じる動作を入れる */
void	IOMultiplexing::_recvRequest(int fd)
{
	char	buffer[BUFF_SIZE + 1];
	int		accepted_socket = fd; // わかりやすくするために代入した.技術的な意味はない
	ssize_t	recved_len = IOM_utils::recvRequest(accepted_socket, buffer);

	if (recved_len < 1)
	{
		debug("recv() < 0: client closed socket");
		IOMultiplexing::_eraseMMAndCloseFd(accepted_socket, &this->_master_readfds);
		std::cout << "accepted_socket: " << fd << ", max_descripotor: " 
								<< this->_max_descripotor << std::endl;
	}
	REQUEST_MESSAGE += buffer;
	if (PARSE_REQUEST_MESSAGE == CONTINUE)
	{
		return ;
	}
	t_response_message	response_message;
	response_message.connection_flg = CONNECTION_CLOSE;

	this->_fd_MessageManagement[accepted_socket].MessageManagement::searchServer(accepted_socket, this->_servers);
	FD_CLR(accepted_socket, &this->_master_readfds);
	FD_SET(accepted_socket, &this->_master_writefds);
	int	cgi_flg = MAKE_RESPONSE_MESSAGE;
	debug(this->_fd_MessageManagement[accepted_socket]);
	if (cgi_flg == CGI_write)
	{
		IOMultiplexing::_switchToWriteCGI(accepted_socket);
	}
	else if (cgi_flg == CGI_read_header)
	{
		IOMultiplexing::_switchToReadCGIResponse(accepted_socket);
	}
	else
	{
		IOMultiplexing::_switchToSendResponse(accepted_socket, response_message);
	}
}

bool	IOMultiplexing::_isCGIWriteFd(int write_fd)
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

bool	IOMultiplexing::_isCGIReadFd(int read_fd)
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

void	IOMultiplexing::_readCGIResponse(int read_fd)
{
	int	accepted_socket = this->_pipefd_fd[read_fd];
	t_response_message	response_message;
	response_message.connection_flg = CONNECTION_CLOSE;
	
	if (this->_fd_MessageManagement[accepted_socket].readCGIResponse(response_message) == CONTINUE)
	{
		return ;
	}
	// CGI の関数でread_fdはclose()した.
	FD_CLR(read_fd, &this->_master_readfds);
	this->_pipefd_fd.erase(READ_FD);
	IOMultiplexing::_decrementMaxDescripotor(read_fd);
	IOMultiplexing::_switchToSendResponse(accepted_socket, response_message);
}

void	IOMultiplexing::_writeCGI(int write_fd) //　エラーの場合、レスポンスメッセージを書く
{
	int	accepted_socket = this->_pipefd_fd[write_fd];
	t_response_message	response_message;
	response_message.connection_flg = CONNECTION_CLOSE;
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
		IOMultiplexing::_switchToSendResponse(accepted_socket, response_message);
	}
	else if (status == END)
	{
		IOMultiplexing::_switchToReadCGIResponse(accepted_socket);
	}
}

void	IOMultiplexing::_closeNotListeningSockets()
{
	for (int fd = 0; fd <= this->_max_descripotor; fd++)
	{
		if (IOMultiplexing::_containsListeningSocket(fd))
			continue;
		if (!FD_ISSET(fd, &this->_master_writefds) && !FD_ISSET(fd, &this->_master_readfds))
			continue;
		int accepted_socket = fd;
		utils::x_close(fd);
		if (METHOD_P)
			delete METHOD_P;
	}
	this->_fd_MessageManagement.clear();
	this->_pipefd_fd.clear();
	FD_ZERO(&this->_master_writefds);
	IOMultiplexing::_initMasterReadfds();
	debug("timeout");
}

void	IOMultiplexing::IOMultiplexingLoop()
{
	int	ready;

	while(true)
	{
		std::memcpy(&this->_writefds, &this->_master_writefds, sizeof(this->_master_writefds));
		std::memcpy(&this->_readfds, &this->_master_readfds, sizeof(this->_master_readfds));
		this->_timeout.tv_sec = 60;
		this->_timeout.tv_usec = 0;
		ready = select(this->_max_descripotor + 1, &this->_readfds, &this->_writefds, NULL, &this->_timeout);
		if (ready == 0)
		{
			IOMultiplexing::_closeNotListeningSockets();
		}
		else if (ready == -1)
			utils::exitWithPutError("select() failed");
		else
		{
			for (int fd = 0; fd < this->_max_descripotor + 1; fd++)
			{
				if (FD_ISSET(fd, &this->_writefds))
				{
					if (IOMultiplexing::_isCGIWriteFd(fd))
					{	
						IOMultiplexing::_writeCGI(fd);
					}
					else if (ATTRIBUTION == NOT_CGI)
					{
						IOMultiplexing::_sendResponse(fd);
						std::cout << "accepted_socket: " << fd << ", max_descripotor: " 
								<< this->_max_descripotor << std::endl;
					}
				}
				else if (FD_ISSET(fd, &this->_readfds))
				{
					if (IOMultiplexing::_containsListeningSocket(fd))
					{
						IOMultiplexing::_createAcceptedSocket(fd);
					}
					else if (IOMultiplexing::_isCGIReadFd(fd))
					{
						IOMultiplexing::_readCGIResponse(fd);
					}
					else if (ATTRIBUTION == NOT_CGI)
					{
						IOMultiplexing::_recvRequest(fd);
					}
				}
			}
		}
	}
}

