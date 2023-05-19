
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
#define map_ite_ std::map<int, MessageManagement>::iterator
#define vec_sever_ std::vector<Server>



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

	public:
		IOMultiplexing(const vec_sever_	&servers, const vec_int_ &vec_ports);
		~IOMultiplexing();

		void	createVecListeningSocket(const vec_int_ &vec_ports);
		void	initMasterReadfds();
		void	IOMultiplexingLoop();
		void	sendResponse(int clnt_socket);
		bool	containsListeningSocket(int fd);
		void	createAcceptedSocket(int listening_socket);
		void	storeRequestToMap(int accepted);

};

#endif