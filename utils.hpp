#ifndef utils_hpp
# define utils_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <cerrno>

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define str_ std::string
#define LOCAL_HOST 2130706433 //127.0.0.1
#define debug(str) std::cout << str << std::endl

enum e_phase
{
	RECV_REQUEST,
	RECV_ENTITY,
	SEND_RESPONSE,
	CGI_PHASE,
	CLOSE_ACCEPTED_SOCKET
};

namespace utils
{
	template <class T> str_ to_string(const T& target)
	{
		std::ostringstream oss;

		oss << target;
		return oss.str();
	}

	void	putError(str_ error_str);
	void	exitWithPutError(str_ error_str);
	int		x_socket(int domain, int type, int protocol);
	void	x_close(int serv_socket, int line);
	void	x_close(int serv_socket);
	void	setSockaddr_in(int port, struct sockaddr_in *addr);
	int		x_setsockopt(int serv_socket, int level, int optname);
	int		x_bind(int serv_socket, struct sockaddr_in addr, socklen_t addr_len);
	int		x_fcntl(int fd, int cmd, int flg);
	int		x_listen(int serv_socket, int backlog);
	int		createListeningSocket(int port);
	bool	recvRequest(int accepted_socket, char *buffer);
	str_	makeResponseMessage(str_ &entity_body);

}

#endif