
#ifndef IOMultiplexing_hpp
# define IOMultiplexing_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>

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

#include "./utils.hpp"

#define str_ std::string
#define vec_int_ std::vector<int>
#define map_fd_data_ std::map<int, t_data>
#define map_ite_ std::map<int, t_data>::iterator

typedef struct s_data
{
	str_	response_message;
	str_	request_message;
}	t_data;

class IOMultiplexing
{
	private:
		IOMultiplexing();
		IOMultiplexing(const IOMultiplexing &io);
		IOMultiplexing &operator=(const IOMultiplexing &rhs);

		int			_max_descripotor;
		vec_int_	_vec_listening_socket;

		fd_set	_master_readfds;
		fd_set	_master_writefds;
		fd_set	_readfds;
		fd_set	_writefds;
	
		struct timeval		_timeout;
		map_fd_data_		_fd_data;

	public:
		IOMultiplexing(std::vector<int> vec_ports);
		~IOMultiplexing();

		void	createVecListeningSocket(std::vector<int> vec_ports);
		void	initMasterReadfds();
		void	IOMultiplexingLoop();
		void	sendResponse(int clnt_socket);
		bool	containsListeningSocket(int fd);
		void	createAcceptedSocket(int listening_socket);
		void	storeRequestToMap(int accepted);

};

#endif