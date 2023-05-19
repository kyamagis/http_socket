#include "../includes/MessageManagement.hpp"

#define REQUEST_PHASE this->_fd_MessageManagement[accepted_socket].request_phase
#define REQUEST_PARSE_LINE_AND_HEADERS this->_fd_MessageManagement[accepted_socket].parseRequestLineAndHeaders(entity_body_pos)
#define REQUEST_INIT_CLASS this->_fd_MessageManagement[accepted_socket].initResponseClass()
#define REQUEST_PARSE_ENTITY_BODY this->_fd_MessageManagement[accepted_socket].parseRequestEntityBody()

MessageManagement::MessageManagement(): Request()
{

}
	
MessageManagement::~MessageManagement()
{

}

MessageManagement::MessageManagement(const MessageManagement &t): Request(), Response()
{
	*this = t;
}

MessageManagement &MessageManagement::operator=(const MessageManagement &rhs)
{
	if (this != &rhs)
	{
		this->request_phase        = rhs.request_phase;
		this->request_message      = rhs.request_message;
		this->deq_method = rhs.deq_method;
	}
	return *this;
}

uint16_t	getPortFromAcceptedSocket(int accepted_socket)
{
	struct sockaddr_in	addr;
    socklen_t			addr_len = sizeof(addr);

	std::memset(&addr, 0, sizeof(sockaddr));
	if (getsockname(accepted_socket, (struct sockaddr *)&addr, &addr_len) == -1)
	{
		utils::putError("getsockname() faile");
		return (ntohs(80));
	}
	return ntohs(addr.sin_port);
}


/* 
	ポート番号とホスト名が一致したサーバーを返す。
	ポート番号が一致するが、ホスト名が一致しない場合、
	ポート番号が一致するベクターの最後のサーバーを返す.
*/

Server	MessageManagement::searchServerWithMatchingPortAndHost(int accepted_socket, const vec_sever_ &servers)
{
	uint16_t	accepted_socket_port = getPortFromAcceptedSocket(accepted_socket);
	size_t		maching_server_idx = 0;

	for (size_t i = 0; i < servers.size(); i++)
	{
		if (accepted_socket_port == servers[i].listen_port.getValue())
		{
			maching_server_idx = i;
			if (this->host.getValue()== servers[i].listen_host.getValue())
			{
				return servers[i];
			}
		}
	}
	return servers[maching_server_idx];
}

int MessageManagement::storeMethodToDeq()
{
	if (this->status_code != 200)
	{
		this->deq_method.push_back(ErrorRequest(*this, this->status_code));
		return this->status_code;
	}
	else if (this->method == "GET")
	{
		this->deq_method.push_back(GET(*this));		
		return 200;
	}
	else if (this->method == "POST")
	{
		this->deq_method.push_back(POST(*this));
		return 200;
	}
	else if (this->method == "DELETE")
	{
		this->deq_method.push_back(DELETE(*this));
		return 200;
	}
	this->deq_method.push_back(ErrorRequest(*this, 405));
	return 405;
}


t_response_message	MessageManagement::makeResponseMessage(int accepted_socket, const vec_sever_ &servers)
{
	// sigchildを無視することで、子プロセスの終了を親プロセスが待たないようにする。<= やる意味がわからない
	// これを実行すると、waitpid()からexecve()の失敗を検知できない。それでもいいのか？
	// signal(SIGCHLD, SIG_IGN);
	t_response_message	response_message;

	response_message.connection_flg = CONNECTION_CLOSE;

	Server	server = MessageManagement::searchServerWithMatchingPortAndHost(accepted_socket, servers);
	int		local_status_code = MessageManagement::storeMethodToDeq();

	if (local_status_code == 200)
	{
		local_status_code = this->deq_method.back().exeMethod(server);
	}
	if (local_status_code == 301)
	{
		response_message.response_message = Response::redirectionResponseMessage(this->deq_method.back().getResponse_redirect_uri(),
													server);
		return response_message;
	}
	if (local_status_code != 200)
	{
		response_message.response_message = Response::errorResponseMessage(local_status_code, server);
		return response_message;
	}
	if (this->deq_method.back().connection.getValue() == CONNECTION_KEEP_ALIVE)
		response_message.connection_flg = CONNECTION_KEEP_ALIVE;
	response_message.response_message = Response::okResponseMessage(local_status_code,
																	this->deq_method.back().getResponse_entity_body(),
																	 this->deq_method.back().getContentType(),
																	 this->deq_method.back().connection,
																	server);
	return response_message;
}



std::ostream &operator<<(std::ostream &ostrm, const MessageManagement &req)
{
	ostrm << "++++++++++++++++++++++++++++++++++" << std::endl;

	ostrm << req.method << " " << req.uri << " " << req.version << std::endl;

	ostrm << "Host:              " << req.host.getValue() << std::endl;
	if (req.port != -1)
		ostrm << "port:              " << req.port << std::endl;
	if (req.content_type.getStatus())
		ostrm << "Content-Type:      " << req.content_type.getValue() << std::endl;
	if (req.content_length.getStatus())
		ostrm << "Content-length:    " << req.content_length.getValue() << std::endl;
	if (req.transfer_encoding.getStatus())
		ostrm << "Transfer-Encoding: " << req.transfer_encoding.getValue() << std::endl;
	if (req.connection.getStatus())
	{
		ostrm << "Connection" << req.transfer_encoding.getValue() << std::endl;
	}
	if (0 < req.request_entity_body.size())
		ostrm << std::endl
			  << req.request_entity_body << std::endl;
	ostrm << "++++++++++++++++++++++++++++++++++" << std::endl;
	return ostrm;
}
