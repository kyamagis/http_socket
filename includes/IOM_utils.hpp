#ifndef IOM_utils_hpp
# define IOM_utils_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <cerrno>
#include <cstring>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
// #include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "./utils.hpp"

#define str_ std::string
#define vec_str_ std::vector<str_>
#define LOCAL_HOST 2130706433 //127.0.0.1
#define debug(str) std::cout << str << std::endl

namespace IOM_utils
{
	int		x_socket(int domain, int type, int protocol);
	void	setSockaddr_in(int port, struct sockaddr_in *addr);
	int		x_setsockopt(int serv_socket, int level, int optname);
	int		x_bind(int serv_socket, struct sockaddr *ai_addr, socklen_t ai_addrlen);
	int		x_listen(int serv_socket, int backlog);
	int		createListeningSocket(const char *host, int port);
	bool	recvRequest(int accepted_socket, char *buffer);
}

#endif
