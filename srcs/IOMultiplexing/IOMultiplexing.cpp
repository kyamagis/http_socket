#include "../../includes/IOMultiplexing.hpp"

#define BUFF_SIZE 10240
#define DEQ_RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].deq_response_message
#define RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].deq_response_message[0].response_message
#define MAKE_RESPONSE_MESSAGE this->_fd_MessageManagement[accepted_socket].makeResponseMessage(accepted_socket, this->_servers)
#define METHOD_P this->_fd_MessageManagement[accepted_socket].method_p
#define REQUEST_MESSAGE this->_fd_MessageManagement[accepted_socket].request_message
#define PARSE_REQUEST_MESSAGE this->_fd_MessageManagement[accepted_socket].parseRequstMessage()
#define INIT_REQUEST_CLASS this->_fd_MessageManagement[accepted_socket].initResponseClass()


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
		if (accepted_socket == this->_max_descripotor)
		{	
			while (!FD_ISSET(this->_max_descripotor, &this->_master_readfds) && \
					!FD_ISSET(this->_max_descripotor, &this->_master_writefds))
				this->_max_descripotor -= 1;
		}
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
		FD_SET(accepted_socket, &this->_master_readfds);
		if (this->_max_descripotor < accepted_socket)
			this->_max_descripotor = accepted_socket;
	}
}

/* 基底回数以上になったら、閉じる動作を入れる */
void	IOMultiplexing::storeRequestToMap(int accepted_socket)
{
	char	buffer[BUFF_SIZE + 1];

	if (!IOM_utils::recvRequest(accepted_socket, buffer))
		return ;

	REQUEST_MESSAGE += buffer;
	if (PARSE_REQUEST_MESSAGE == END)
	{
		FD_CLR(accepted_socket, &this->_master_readfds);
		FD_SET(accepted_socket, &this->_master_writefds);
		DEQ_RESPONSE_MESSAGE.push_back(MAKE_RESPONSE_MESSAGE);
		debug(this->_fd_MessageManagement[accepted_socket]);
		INIT_REQUEST_CLASS;
		delete METHOD_P;
		METHOD_P = NULL;
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
					IOMultiplexing::sendResponse(fd);
					std::cout << "clnt_socket: " <<  fd << ", max_descripotor: " 
								<< this->_max_descripotor << std::endl;
				}
				else if (FD_ISSET(fd, &this->_readfds))
				{
					if (IOMultiplexing::containsListeningSocket(fd))
					{
						IOMultiplexing::createAcceptedSocket(fd);
					}
					else
					{
						IOMultiplexing::storeRequestToMap(fd);
					}
				}
			}
		}
	}
}

