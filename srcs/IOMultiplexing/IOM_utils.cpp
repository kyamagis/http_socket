
#include "../../includes/IOM_utils.hpp"

#define vec_str_ std::vector<str_>

namespace IOM_utils
{
	int	x_socket(int domain, int type, int protocol)
	{
		int	serv_socket = socket(domain, type, protocol);
		if (serv_socket == -1)
		{
			utils::putError("socket() failed");
			return -1;
		}
		return serv_socket;
	}

	void	setSockaddr_in(int port, struct sockaddr_in *addr)
	{
		std::memset(addr, 0, sizeof(*addr));
		addr->sin_addr.s_addr		= htonl(LOCAL_HOST);
		addr->sin_family			= AF_INET;
		addr->sin_port				= htons(port);
	}

	int	x_setsockopt(int serv_socket, int level, int optname)
	{
		int	option_value = 1;

		if (setsockopt(serv_socket, level, optname, (const char *)&option_value, sizeof(option_value)) == -1)
		{
			utils::putError("setsockopt() failed");
			utils::x_close(serv_socket);
			return -1;
		}
		return 0;
	}

	int	x_bind(int serv_socket, struct sockaddr *ai_addr, socklen_t ai_addrlen)
	{
		if (bind(serv_socket, ai_addr, ai_addrlen) == -1)
		{
			//utils::putError("bind() failed");
			return -1;
		}
		return 0;
	}

	int	x_listen(int serv_socket, int backlog)
	{
		if (listen(serv_socket, backlog) == -1)
		{
			utils::putError("listen() failed");
			utils::x_close(serv_socket);
			return -1;
		}
		return 0;
	}

	// https://man7.org/linux/man-pages/man3/getaddrinfo.3.html

	int	x_getaddrinfo(const char *host, const char *port, struct addrinfo **result)
	{
		struct addrinfo hints;
		int	val;

		std::memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV;
		val = getaddrinfo(host, utils::to_string(port).c_str(), &hints, result);
		if (val != 0)
		{
			std::cerr << "getaddrinfo() fail: " << gai_strerror(val) << std::endl;
			return -1;
		}
		return 0;
	}

	int	createListeningSocket(const char *host, int port)
	{
		struct addrinfo	*result;
		struct addrinfo	*tmp;
		
		if (x_getaddrinfo(host, utils::to_string(port).c_str(), &result) != 0)
		{
			return -1;
		}
		int	serv_socket;
		for (tmp = result; tmp != NULL; tmp = tmp->ai_next)
		{
			serv_socket = x_socket(AF_INET, SOCK_STREAM, 0);
			if (serv_socket == -1)
				continue;
			if (x_setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR) == -1)
			{
				freeaddrinfo(result);
				return -1;
			}
			if (x_bind(serv_socket, tmp->ai_addr, tmp->ai_addrlen) == 0)
				break;
			utils::x_close(serv_socket);
		}
		freeaddrinfo(result);
		if (tmp == NULL)
			return -1;
		int error_flg = utils::x_fcntl(serv_socket, F_SETFL, O_NONBLOCK);
		if (error_flg == -1)
			return -1;
		error_flg = x_listen(serv_socket, SOMAXCONN);
		if (error_flg == -1)
			return -1;
		std::cout << "host: " << host <<", port: " << port;
		return serv_socket;
	}

	#define BUFF_SIZE 10240 //適当な数値

	ssize_t	recvRequest(int accepted, char *buffer)
	{
		ssize_t recved_len = 1;

		std::memset(buffer, '\0', BUFF_SIZE + 1);
		recved_len = recv(accepted, buffer, BUFF_SIZE, MSG_DONTWAIT);
		if (recved_len == -1)
		{
			if (errno != EWOULDBLOCK)
			{
				utils::exitWithPutError("recv() failed");
			}
			debug("recv == -1");
			return -1;
		}
		if (recved_len == 0)
		{
			return 0;
		}
		return recved_len;
	}
}
