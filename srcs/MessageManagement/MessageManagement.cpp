#include "../../includes/MessageManagement.hpp"

#define REQUEST_PHASE this->_fd_MessageManagement[accepted_socket].request_phase
#define REQUEST_PARSE_LINE_AND_HEADERS this->_fd_MessageManagement[accepted_socket].parseRequestLineAndHeaders(entity_body_pos)
#define REQUEST_INIT_CLASS this->_fd_MessageManagement[accepted_socket].initResponseClass()
#define REQUEST_PARSE_ENTITY_BODY this->_fd_MessageManagement[accepted_socket].parseRequestEntityBody()

MessageManagement::MessageManagement(): Request(), attribution(NOT_CGI), method_p(NULL), parent_fd(0)
{
	
}
	
MessageManagement::~MessageManagement()
{
	if (this->method_p != NULL)
	{
		delete this->method_p;
	}
}

MessageManagement::MessageManagement(const MessageManagement &t): Request(), Response()
{
	*this = t;
}

MessageManagement &MessageManagement::operator=(const MessageManagement &rhs)
{
	if (this != &rhs)
	{
		this->attribution     = rhs.attribution;
		this->method_p = rhs.method_p; // new したほうがいい気がするが
		this->parent_fd       = rhs.parent_fd;
		this->request_phase   = rhs.request_phase;
		this->request_message = rhs.request_message;
	}
	return *this;
}

int MessageManagement::storeMethodToDeq()
{
	if (this->status_code != 200)
	{
		this->method_p = new ErrorRequest(*this, this->status_code);
		return this->status_code;
	}
	else if (this->method == "GET")
	{
		this->method_p = new GET(*this);		
		return 200;
	}
	else if (this->method == "POST")
	{
		this->method_p = new POST(*this);
		return 200;
	}
	else if (this->method == "DELETE")
	{
		this->method_p = new DELETE(*this);
		return 200;
	}
	this->method_p = new ErrorRequest(*this, 405);

	return 405;
}


int	MessageManagement::makeResponseMessage(t_response_message &response_message, int max_descripotor)
{
	// sigchildを無視することで、子プロセスの終了を親プロセスが待たないようにする。<= やる意味がわからない
	// これを実行すると、waitpid()からexecve()の失敗を検知できない。それでもいいのか？
	// signal(SIGCHLD, SIG_IGN);

	int	local_status_code;

	try {
		local_status_code = MessageManagement::storeMethodToDeq();
	}
	catch (std::bad_alloc &err) {
		std::cerr << err.what() << std::endl;
		local_status_code = 500;
		this->method_p = NULL;
	}

	if (local_status_code == 200)
	{
		local_status_code = this->method_p->exeMethod(this->server, max_descripotor);
		if (local_status_code == CGI_write || local_status_code == CGI_read_header)
		{
			return local_status_code;
		}
	}
	if (local_status_code == 301)
	{
		response_message.response_message = Response::redirectionResponseMessage(this->method_p->getResponse_redirect_uri(),
													this->server);
		return 200;
	}
	if (local_status_code != 200)
	{
		response_message.response_message = Response::errorResponseMessage(local_status_code, this->server);
		return 200;
	}
	if (this->method_p->connection.getValue() == CONNECTION_KEEP_ALIVE)
		response_message.connection_flg = CONNECTION_KEEP_ALIVE;
	response_message.response_message = Response::okResponseMessage(local_status_code,
																	this->method_p->getResponse_entity_body(),
																	 this->method_p->getContentType(),
																	 this->method_p->connection,
																	this->server);
	return 200;
}

int		MessageManagement::readCGIResponse(t_response_message &response_message)
{
	int status = this->method_p->cgi.readAndWaitpid();
	
	if (status == CONTINUE)
	{
		return CONTINUE;
	}
	else if (status == END)
	{
		status = this->method_p->endCGI();
		if (status != 200)
		{
			response_message.response_message = Response::errorResponseMessage(status, this->server);
			return END;
		}

		if (this->method_p->connection.getValue() == CONNECTION_KEEP_ALIVE)
			response_message.connection_flg = CONNECTION_KEEP_ALIVE;
		response_message.response_message = Response::okResponseMessage(200,
																	this->method_p->getResponse_entity_body(),
																	 this->method_p->getContentType(),
																	 this->method_p->connection,
																	this->server);
		
		return END;
	}
	// status がエラーだった場合
	response_message.response_message = Response::errorResponseMessage(status, this->server);
	return END;
}

int		MessageManagement::writeCGIRequest(t_response_message &response_message)
{
	int	status = this->method_p->cgi.writeRequestEntityBodyToCGI();

	if (status == CONTINUE)
	{
		return CONTINUE;
	}
	else if (status == END)
	{
		return END;
	}

	// status がエラーだった場合
	response_message.response_message = Response::errorResponseMessage(status, this->server);
	return status;
}

bool	MessageManagement::parseRequstMessage(const vec_sever_ &servers)
{
	return Request::parseRequstMessage(servers);
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
		ostrm << "Connection:        " ;
		if (req.connection.getValue() == 1)
			ostrm << "keep-alive" << std::endl;
		else
			ostrm << "close" << std::endl;
	}
	ostrm << std::endl
		  << req.request_entity_body << std::endl;
	ostrm << "++++++++++++++++++++++++++++++++++" << std::endl;
	return ostrm;
}
