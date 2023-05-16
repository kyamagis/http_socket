#include "./IOMultiplexing.hpp"

#define BUFF_SIZE 10240
#define RESPONSE_MESSAGE this->_fd_data[accepted_socket].response_message

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

IOMultiplexing::IOMultiplexing(std::vector<int> vec_ports)
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


void	IOMultiplexing::createVecListeningSocket(std::vector<int> vec_ports)
{
	int	listening_socket;

	for (size_t i = 0; i < vec_ports.size(); i++)
	{
		listening_socket = utils::createListeningSocket(vec_ports[i]);
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
		this->_fd_data.erase(accepted_socket);
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
		FD_SET(accepted_socket, &this->_master_readfds);
		if (this->_max_descripotor < accepted_socket)
			this->_max_descripotor = accepted_socket;
	}
}


void	IOMultiplexing::storeRequestToMap(int accepted_socket)
{
	char	buffer[BUFF_SIZE + 1];

	if (!utils::recvRequest(accepted_socket, buffer))
		return ;

	map_ite_	ite = this->_fd_data.find(accepted_socket);
	if (ite == this->_fd_data.end())
	{
		this->_fd_data.insert(std::pair<int, t_data>(accepted_socket, t_data()));
	}
	RESPONSE_MESSAGE += buffer;
	if (buffer[BUFF_SIZE - 1] == '\0')
	{
		FD_CLR(accepted_socket, &this->_master_readfds);
		FD_SET(accepted_socket, &this->_master_writefds);
		RESPONSE_MESSAGE = utils::makeResponseMessage(RESPONSE_MESSAGE);
	}
}

void	IOMultiplexing::IOMultiplexingLoop()
{
	int	ready;

	while(true)
	{
		memcpy(&_writefds, &_master_writefds, sizeof(_master_writefds));
		memcpy(&_readfds, &_master_readfds, sizeof(_master_readfds));
		ready = select(_max_descripotor + 1, &_readfds, &_writefds, NULL, &this->_timeout);
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
				if (FD_ISSET(fd, &_writefds))
				{
					sendResponse(fd);
					std::cout << "clnt_socket: " <<  fd << ", max_descripotor: " 
								<< this->_max_descripotor << std::endl;
				}
				else if (FD_ISSET(fd, &_readfds))
				{
					if (containsListeningSocket(fd))
					{
						createAcceptedSocket(fd);
					}
					else
					{
						storeRequestToMap(fd);
					}
				}
			}
		}
	}
}

