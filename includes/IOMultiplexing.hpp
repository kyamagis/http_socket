
#ifndef IOMultiplexing_hpp
# define IOMultiplexing_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstring>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#include "./IOM_utils.hpp"
#include "./utils.hpp"
#include "./MessageManagement.hpp"
#include "./Config.hpp"

#define str_ std::string
#define vec_int_ std::vector<int>
#define map_fd_MessageManagement_ std::map<int, MessageManagement>
#define map_fd_MessageManagement_ite_ std::map<int, MessageManagement>::iterator
#define map_pipefd_fd_ std::map<int, int>
#define map_pipefd_fd_ite_ std::map<int, int>::iterator
#define vec_sever_ std::vector<Server>

class MessageManagement;

class IOMultiplexing
{
	private:
		IOMultiplexing();
		IOMultiplexing(const IOMultiplexing &io);
		IOMultiplexing &operator=(const IOMultiplexing &rhs);

		const vec_sever_	_servers;

		int			_max_descripotor;
		vec_int_	_vec_listening_socket;

		fd_set	_master_readfds;
		fd_set	_master_writefds;
		fd_set	_readfds;
		fd_set	_writefds;
	
		struct timeval				_timeout;
		map_fd_MessageManagement_	_fd_MessageManagement;
		map_pipefd_fd_				_pipefd_fd;

		void	_initMasterReadfds();
		void	_createVecListeningSocket();

		void	_decrementMaxDescripotor(int fd);
		void	_eraseMMAndCloseFd(int accepted_socket, fd_set *fds);

		void	_switchToRecvRequest(int accepted_socket);
		void	_switchToSendResponse(int accepted_socket, const t_response_message &response_message);
		void	_switchToReadCGIResponse(int accepted_socket);
		void	_switchToWriteCGI(int accepted_socket);


		bool	_containsListeningSocket(int fd);
		bool	_isCGIWriteFd(int write_fd);
		bool	_isCGIReadFd(int read_fd);
	
		void	_closeNotListeningSockets();
		void	_sendResponse(int clnt_socket);
		void	_recvRequest(int fd);
		void	_createAcceptedSocket(int listening_socket);
		void	_readCGIResponse(int read_fd);
		void	_writeCGI(int write_fd);

	public:
		IOMultiplexing(const vec_sever_	&servers);
		~IOMultiplexing();

		void	IOMultiplexingLoop();
		
};

#endif
