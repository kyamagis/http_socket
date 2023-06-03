
#include "../../includes/Request.hpp"


Request::Request(): request_phase(RECV_REQUEST), status_code(200), port(-1)
{
	this->connection.setValue(CONNECTION_CLOSE);
	this->chunked_turn = NUM_TURN;
	this->chunked_size = 0;
}

Request::Request(const Request &t)
{
	*this = t;
}

Request &Request::operator=(const Request &rhs)
{
	if (this != &rhs)
	{
		this->request_phase = rhs.request_phase;
		this->location = rhs.location;

		this->status_code = rhs.status_code;
		this->request_message = rhs.request_message;
		this->method = rhs.method;
		this->uri = rhs.uri;
		this->version = rhs.version;

		this->host = rhs.host;
		this->port = rhs.port;

		this->content_type = rhs.content_type;
		this->content_length = rhs.content_length;
		this->transfer_encoding = rhs.transfer_encoding;
		this->connection = rhs.connection;
		this->request_entity_body = rhs.request_entity_body;

		this->chunked_turn = rhs.chunked_turn;
		this->chunked_size = rhs.chunked_size;
	}
	return *this;
}
	
Request::~Request()
{

}

void	Request::initResponseClass()
{
	this->request_phase = RECV_REQUEST;
	this->status_code = 200;
	this->request_message.clear();
	this->method.clear();
	this->uri.clear();
	this->version.clear();
	this->host.clearValueClass("");
	this->port = -1;
	this->content_type.clearValueClass("");
	this->content_length.clearValueClass(0);
	this->transfer_encoding.clearValueClass("");
	this->connection.clearValueClass(0);
	this->request_entity_body.clear();
	this->chunked_turn = NUM_TURN;
	this->chunked_size = 0;
}

vec_str_	Request::_spliteRequestLineAndHeader(size_t entity_body_pos)
{
	str_	request_line_and_header = this->request_message.substr(0, entity_body_pos);

	return utils::split_Str(request_line_and_header, "\r\n");
}


void	Request::_parseRequestLine(const str_ &request_line)
{
	vec_str_	vec_request_line = utils::split_Str(request_line, " ");

	if (vec_request_line.size() != 3) // method uri HTTPversion
	{
		this->status_code = 400;
		return ;
	}
	this->method = vec_request_line[0];
	this->uri = vec_request_line[1];
	if (vec_request_line[2] != "HTTP/1.1" && vec_request_line[2] != "HTTP/1.0")
	{
		this->status_code = 505;
	}
	this->version = vec_request_line[2];
	this->status_code = 200;
	return ;
}

void	Request::_parseHost(const str_ &host_header)
{
	vec_str_	vec_host_header = utils::split_Str(host_header, " ");
	if (vec_host_header.size() != 2) // Host: www.42.ac.jp:8080
	{
		this->status_code = 400;
		return ;
	}
	if (utils::ft_strlwr(vec_host_header[0]) != "host:")
	{	
		this->status_code = 400;
		return ;
	}
	size_t	colon_pos = vec_host_header[1].find(':');
	if (colon_pos == 0)
	{
		this->status_code = 400;
		return ;
	}
	str_	lower_str = utils::ft_strlwr(vec_host_header[1]);
	if (colon_pos != str_::npos)
	{
		this->port = utils::myStrToLL(&lower_str[colon_pos + 1]);
		if (this->port < 0x0 || 0xffff < this->port)
		{
			this->status_code = 400;
			return ;
		}
		lower_str = lower_str.substr(0, colon_pos);
	}
	this->host.setValue(lower_str);
	this->status_code = 200;
	return ;
}

int	Request::_parseMime(const str_ &lower_str, const vec_str_ &vec_split_a_header)
{
	size_t size = lower_str.size();
	size_t i = 0;
	size_t slash_count = 0;
	str_ type_name;

	if (size < 3 || lower_str[0] == '/')
	{
		return 400;
	}
	for (; i < size && lower_str[i] != ';'; i++)
	{
		if (lower_str[i] == '/')
		{
			slash_count++;
		}
	}
	if (slash_count != 1 || lower_str[i - 1] == '/')
	{
		return 400;
	}
	if (lower_str[i] != ';' && vec_split_a_header.size() != 2)
	{
		return 400;
	}
	type_name = lower_str.substr(0, i);
	if (type_name != "image/jpg" && type_name != "text/html" && type_name != "text/css")
	{
		this->content_type.setValue((str_) "text/plain");
	}
	else
	{
		this->content_type.setValue(type_name);
	}
	return 200;
}

void Request::_parseContentType(const vec_str_ &vec_split_a_header)
{
	if (this->content_type.getStatus() != NOT_SET)
	{
		this->status_code = 400;
		return ;
	}
	str_ lower_str = utils::ft_strlwr(vec_split_a_header[1]);
	if (Request::_parseMime(lower_str, vec_split_a_header) == 400)
	{
		this->status_code = 400;
		return ;
	}
}

void	Request::_parseContentLength(const vec_str_ &vec_split_header)
{
	if (this->content_length.getStatus() != NOT_SET || this->transfer_encoding.getStatus() != NOT_SET)
	{
		this->status_code = 400;
		return ;
	}
	if (vec_split_header.size() != 2)
	{
		this->status_code = 400;
		return ;
	}
	long value = utils::myStrToLL(vec_split_header[1]);
	if (value < 0)
	{
		this->status_code = 400;
		return ;
	}
	this->content_length.setValue((unsigned int)value);
}

void	Request::_parseTransferEncoding(const vec_str_ &vec_split_a_header)
{
	if (this->content_length.getStatus() != NOT_SET || this->transfer_encoding.getStatus() != NOT_SET)
	{
		this->status_code = 400;
		return ;
	}
	if (vec_split_a_header.size() != 2)
	{
		this->status_code = 400;
		return ;
	}
	if (utils::ft_strlwr(vec_split_a_header[1]) != "chunked")
	{
		if (this->status_code == 200)
			this->status_code = 501;
		return ;
	}
	this->transfer_encoding.setValue((str_) "chunked");
}

void	Request::_parseConnection(const vec_str_ &vec_split_a_header)
{
	if (this->connection.getValue() != NOT_SET)
	{
		this->status_code = 400;
		return ;
	}
	if (vec_split_a_header.size() != 2)
	{
		this->status_code = 400;
		return ;
	}
	if (utils::ft_strlwr(vec_split_a_header[1]) == "close")
	{
		this->connection.setValue(CONNECTION_CLOSE);
		return ;
	}
	else if (utils::ft_strlwr(vec_split_a_header[1]) == "keep-alive")
	{
		this->connection.setValue(CONNECTION_KEEP_ALIVE);
		return ;
	}
	else
	{
		this->status_code = 400;
		return ;
	}
}

void	Request::_parseHeaders(const vec_str_ &request_headers)
{
	vec_str_	vec_split_a_header;
	size_t		i = 2;
	str_		lower_str;

	for (; i < request_headers.size(); i++)
	{
		vec_split_a_header = utils::split_Str(request_headers[i], ": ");

		lower_str = utils::ft_strlwr(vec_split_a_header[0]);
		if (lower_str == "content-type" && 1 < vec_split_a_header.size())
		{
			Request::_parseContentType(vec_split_a_header);
		}
		else if (lower_str == "content-length" && 1 < vec_split_a_header.size())
		{
			Request::_parseContentLength(vec_split_a_header);
		}
		else if (lower_str == "transfer-encoding" && 1 < vec_split_a_header.size())
		{
			Request::_parseTransferEncoding(vec_split_a_header);
		}
		else if (lower_str == "connection" && 1 < vec_split_a_header.size())
		{
			Request::_parseConnection(vec_split_a_header);
		}
	}
}

int	Request::_setLocation(const Server& server)
{
	std::vector<Location>	locations = server.locations.getValue();
	size_t					size = locations.size();
	str_					location_path;
	size_t					location_length;
	size_t					matching_location_length = 0;

	for (size_t i = 0; i < size; i++) {
		location_path = locations[i].path.getValue();
		if (this->uri.find(location_path) != 0)
			continue ;
		location_length = location_path.length();
		if (matching_location_length <= location_length) {
			this->location = locations[i];
			matching_location_length = location_length;
		}
	}
	return 200;
}

bool	Request::_parseRequestLineAndHeaders(size_t entity_body_pos, const Server &server)
{
	vec_str_	vec_splite_request = Request::_spliteRequestLineAndHeader(entity_body_pos + 2);
	if (vec_splite_request.size() < 2)
	{
		this->status_code = 400;
	}

	/* entity bodyのみを保持するようにする */
	this->request_message = this->request_message.substr(entity_body_pos + 4);

	/* リクエストラインとヘッダーのパース */
	if (this->status_code == 200)
		Request::_parseRequestLine(vec_splite_request[0]);
	if (this->status_code == 200)
		Request::_parseHost(vec_splite_request[1]);
	Request::_parseHeaders(vec_splite_request);
	Request::_setLocation(server);
	if (this->content_length.getStatus() == NOT_SET && \
		this->transfer_encoding.getStatus() == NOT_SET)
	{
		return END;
	}
	return CONTINUE;
}

#define KEEP_ON 2

int	Request::_storeChunkedStr()
{
	size_t	substr_len = this->chunked_size;

	if (this->request_message.size() < this->chunked_size)
	{
		substr_len = this->request_message.size();
	}
	this->chunked_size -= substr_len;
	this->request_entity_body += this->request_message.substr(0, substr_len);
	if (this->location.client_max_body_size.getValue() < this->request_entity_body.size())
	{
		this->status_code = 413;
		return END;
	}
	this->request_message = this->request_message.substr(substr_len);
	if (this->chunked_size == 0)
	{
		this->chunked_turn = NUM_TURN;
		size_t	cr_lf_pos = this->request_message.find("\r\n");
		if (cr_lf_pos != 0)
		{
			this->status_code = 400;
			return END;
		}
		this->request_message = this->request_message.substr(cr_lf_pos + 2);
	}
	return KEEP_ON;
}

int	Request::_gainChunkSize()
{
	str_	chunked_str;
	size_t	cr_lf_pos;
	bool	over_flow = false;

	cr_lf_pos = this->request_message.find("\r\n");
	if (cr_lf_pos == str_::npos)
	{
		return CONTINUE;
	}
	chunked_str = this->request_message.substr(0, cr_lf_pos);
	this->request_message = this->request_message.substr(cr_lf_pos + 2);
	this->chunked_size = request_utils::hexStrToLL(chunked_str, over_flow);
	if (over_flow)
	{
		this->status_code = 400;
		return END;
	}
	if (this->chunked_size == 0)
	{
		return END;
	}
	this->chunked_turn = STR_TURN;
	return KEEP_ON;
}

bool	Request::_storeEntityBodyChunked()
{
	
	while (0 < this->request_message.size())
	{
		if (this->chunked_turn == NUM_TURN)
		{
			int	to_be_or_not_to_be = Request::_gainChunkSize();
			if (to_be_or_not_to_be == CONTINUE)
				return CONTINUE;
			else if (to_be_or_not_to_be == END)
				return END;
		}
		else if (this->chunked_turn == STR_TURN)
		{
			if (Request::_storeChunkedStr() == END)
				return END;
		}
	}
	return CONTINUE;
}

/* 
	chunkedでない場合、entity bodyは、\r\nでパースせず,
	単純にcontent-length で指定された文字数分そのまま受け取る.
	content-length: 5の場合で、送られてきたentitybody が abc\r\nの場合、
	受け取るのは、abc\r\n
*/

bool Request::_storeEntityBodyContentLength()
{
	this->request_entity_body += this->request_message.substr(0, this->content_length.getValue());
	if (this->location.client_max_body_size.getValue() < this->request_entity_body.size())
	{
		this->status_code = 413;
		return END;
	}
	if (this->request_message.size() < this->content_length.getValue())
	{
		this->request_message.clear();
	}
	else
	{
		this->request_message = this->request_message.substr(this->content_length.getValue());
	}
	if (this->request_entity_body.size() < this->content_length.getValue())
	{
		this->content_length.setValue(this->content_length.getValue() - this->request_entity_body.size());
		return CONTINUE;
	}
	return END;
}

bool	Request::_parseRequestEntityBody()
{
	if (this->content_length.getStatus() != NOT_SET)
	{
		return Request::_storeEntityBodyContentLength();
	}
	else if (this->transfer_encoding.getStatus() != NOT_SET)
	{
		return Request::_storeEntityBodyChunked();
	}
	return END;
}

bool	Request::parseRequstMessage(const Server &server)
{
	size_t	entity_body_pos = this->request_message.find("\r\n\r\n");

	if (this->request_phase == RECV_ENTITY_BODY)
	{
		if (Request::_parseRequestEntityBody() == END) //error あるいは、このrecvで期待しているバイト数を得られた場合
		{
			this->request_phase = RECV_REQUEST;
			return END;
		}
	}
	else if  (entity_body_pos != str_::npos)//エンティティボディの検知
	{
		if (Request::_parseRequestLineAndHeaders(entity_body_pos, server) == END) // content-length, transfer-encoding が存在しない場合
		{
			return END;
		}
		if (Request::_parseRequestEntityBody() == END) //error あるいは、このrecvで期待しているバイト数を得られた場合
		{
			return END;
		}
		this->request_phase = RECV_ENTITY_BODY;
	}
	return CONTINUE;
}


