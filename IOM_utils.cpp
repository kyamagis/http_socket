
#include "./IOM_utils.hpp"

#define vec_str_ std::vector<str_>

namespace utils
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

	void	x_close(int serv_socket, int line)
	{
		int error_flg = close(serv_socket);
		if (error_flg == -1)
		{
			std::cerr << "close() failed : " 
						<< strerror(errno) 
						<< " : line = " << line << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}

	void	x_close(int serv_socket)
	{
		int error_flg = close(serv_socket);
		if (error_flg == -1)
		{
			utils::exitWithPutError("close() failed");
		}
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
			x_close(serv_socket);
			return -1;
		}
		return 0;
	}

	int	x_bind(int serv_socket, struct sockaddr_in addr, socklen_t addr_len)
	{
		if (bind(serv_socket, (struct sockaddr *)&addr, addr_len) == -1)
		{
			x_close(serv_socket);
			return -1;
		}
		return 0;
	}

	int	x_fcntl(int fd, int cmd, int flg)
	{
		if (fcntl(fd, cmd, flg) == -1)
		{
			utils::putError("fcntl() failed");
			x_close(fd);
			return -1;
		}
		return 0;
	}

	int	x_listen(int serv_socket, int backlog)
	{
		if (listen(serv_socket, backlog) == -1)
		{
			utils::putError("listen() failed");
			x_close(serv_socket);
			return -1;
		}
		return 0;
	}

	int	createListeningSocket(int port)
	{
		struct sockaddr_in	serv_addr;

		int serv_socket = x_socket(AF_INET, SOCK_STREAM, 0);
		if (serv_socket == -1)
			return -1;
		setSockaddr_in(port, &serv_addr);
		int	error_flg = x_setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR);
		if (error_flg == -1)
			return -1;
		error_flg = x_bind(serv_socket, serv_addr, sizeof(serv_addr));
		if (error_flg == -1)
			return -1;
		error_flg = x_fcntl(serv_socket, F_SETFL, O_NONBLOCK);
		if (error_flg == -1)
			return -1;
		error_flg = x_listen(serv_socket, SOMAXCONN);
		if (error_flg == -1)
			return -1;
		return serv_socket;
	}

	#define BUFF_SIZE 10240 //適当な数値

	bool	recvRequest(int accepted, char *buffer)
	{
		ssize_t recved_len = 1;

		std::memset(buffer, '\0', BUFF_SIZE + 1);
		recved_len = recv(accepted, buffer, BUFF_SIZE, MSG_DONTWAIT);
		if (recved_len == -1)
		{
			if (errno != EWOULDBLOCK)
			{
				utils::exitWithPutError("accept() failed");
			}
			debug("recv == -1");
			return false;
		}
		return true;
	}

	str_	makeResponseMessage(str_ &entity_body)
	{
		str_	response_message = "HTTP/1.1 200 OK\r\n";
		response_message += "Connection: close\r\n";
		response_message += "Content-Type: text/plane\r\n";
		response_message += "Content-Length: " + utils::to_string(entity_body.size()) + "\r\n\r\n";
		response_message += entity_body;

		return response_message;
	}

}