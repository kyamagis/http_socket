
#include "./Request.hpp"


Request::Request(): status_code(200), port(-1), connection(CONNECTION_CLOSE)
{
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
		this->status_code = 200;
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
		this->entity_body = rhs.entity_body;

		this->chunked_turn = rhs.chunked_turn;
		this->chunked_size = rhs.chunked_size;
	}
}
	
Request::~Request()
{

}

void	Request::initResponseClass()
{
	this->request_message.clear();
	this->method.clear();
	this->uri.clear();
	this->version.clear();
	this->host.clearValueClass();
	this->port = -1;
	this->content_type.clearValueClass();
	this->content_length.clearValueClass();
	this->transfer_encoding.clearValueClass();
	this->connection.clearValueClass();
	this->entity_body.clear();
	this->chunked_turn = NUM_TURN;
	this->chunked_size = 0;
}

vec_str_	Request::spliteRequestLineAndHeader(size_t entity_body_pos)
{
	str_	request_line_and_header = this->request_message.substr(0, entity_body_pos);

	return utils::splitStr(request_line_and_header, "\r\n");
}


void	Request::parseRequestLine(const str_ &request_line)
{
	vec_str_	vec_request_line = utils::splitStr(request_line, " ");

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

void	Request::parseHost(const str_ &host_header)
{
	vec_str_	vec_host_header = utils::splitStr(host_header, " ");
	if (vec_host_header.size() != 2) // Host: www.42.ac.jp:8080
	{
		this->status_code = 400;
		return ;
	}
	if (request_utils::ft_strlwr(vec_host_header[0]) != "host:")
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
	str_	lower_str = request_utils::ft_strlwr(vec_host_header[1]);
	if (colon_pos != str_::npos)
	{
		this->port = request_utils::myStrToLL(&lower_str[colon_pos + 1]);
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

int	Request::parseMime(const str_ &lower_str, const vec_str_ &vec_split_a_header)
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

void Request::parseContentType(const vec_str_ &vec_split_a_header)
{
	if (this->content_type.getStatus() != NOT_SET)
	{
		this->status_code = 400;
		return ;
	}
	str_ lower_str = request_utils::ft_strlwr(vec_split_a_header[1]);
	if (Request::parseMime(lower_str, vec_split_a_header) == 400)
	{
		this->status_code = 400;
		return ;
	}
}

void	Request::parseContentLength(const vec_str_ &vec_split_header)
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
	long value = request_utils::myStrToLL(vec_split_header[1]);
	if (value < 0)
	{
		this->status_code = 400;
		return ;
	}
	this->content_length.setValue((unsigned int)value);
}

void	Request::parseTransferEncoding(const vec_str_ &vec_split_a_header)
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
	if (request_utils::ft_strlwr(vec_split_a_header[1]) != "chunked")
	{
		if (this->status_code == 200)
			this->status_code = 501;
		return ;
	}
	this->transfer_encoding.setValue((str_) "chunked");
}

void	Request::parseConnection(const vec_str_ &vec_split_a_header)
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
	if (request_utils::ft_strlwr(vec_split_a_header[1]) == "close")
	{
		this->connection.setValue(CONNECTION_CLOSE);
		return ;
	}
	else if (request_utils::ft_strlwr(vec_split_a_header[1]) != "keep-alive")
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

void	Request::parseHeaders(const vec_str_ &request_headers)
{
	vec_str_	vec_split_a_header;
	size_t		i = 2;
	str_		lower_str;

	for (; i < request_headers.size(); i++)
	{
		vec_split_a_header = utils::splitStr(request_headers[i], ": ");
		if (vec_split_a_header.size() < 2)
		{
			this->status_code = 400;
			continue ;
		}
		lower_str = request_utils::ft_strlwr(vec_split_a_header[0]);
		if (lower_str == "content-type")
		{
			Request::parseContentType(vec_split_a_header);
		}
		else if (lower_str == "content-length")
		{
			Request::parseContentLength(vec_split_a_header);
		}
		else if (lower_str == "transfer-encoding")
		{
			Request::parseTransferEncoding(vec_split_a_header);
		}
		else if (lower_str == "connection")
		{
			Request::parseConnection(vec_split_a_header);
		}
	}
}


bool	Request::parseRequestLineAndHeaders(size_t entity_body_pos)
{
	vec_str_	vec_splite_request = Request::spliteRequestLineAndHeader(entity_body_pos + 2);
	if (vec_splite_request.size() < 2)
	{
		this->status_code = 400;
	}

	/* entity bodyのみを保持するようにする */
	this->request_message = this->request_message.substr(entity_body_pos + 4);

	/* リクエストラインとヘッダーのパース */
	if (this->status_code == 200)
		Request::parseRequestLine(vec_splite_request[0]);
	if (this->status_code == 200)
		Request::parseHost(vec_splite_request[1]);
	Request::parseHeaders(vec_splite_request);
	if (this->content_length.getStatus() == NOT_SET && \
		this->transfer_encoding.getStatus() == NOT_SET)
	{
		return END;
	}
	CONTINUE;
}

// bool Request::storeEntityBodyChunked()
// {
// 	size_t entity_body_size = this->raw_entity_body.size();
// 	size_t specified_len = 1;
// 	bool over_flow = false;
// 	str_ request_entity_body;

// 	for (size_t i = 0; i < entity_body_size; i++)
// 	{
// 		specified_len = request_utils::hexStrToLL(this->raw_entity_body, i, over_flow);
// 		++i;
// 		if (specified_len == 0 || i == entity_body_size || over_flow == true)
// 		{
// 			break;
// 		}
// 		request_entity_body += this->raw_entity_body.substr(i, specified_len);
// 		for (size_t j = 0; i < entity_body_size && j < specified_len; i++, j++)
// 			;
// 		if (i == entity_body_size)
// 		{
// 			break;
// 		}
// 	}
// 	if (specified_len != 0 || over_flow)
// 	{
// 		return 400;
// 	}
// 	this->_request_entity_body.setValue(request_entity_body);
// 	return 200;
// }

/* 
	content-length 10 ; chunk_size 100 => error
	chunk_size -1 => error
	chunk_size z => error
	chunk_size z => error
*/

bool Request::storeEntityBodyChunked()
{
	str_	chunked_str;
	size_t	cr_lf_pos;
	bool	over_flow;

	for(; this->request_message.size() && this->content_length.getValue();)
	{
		cr_lf_pos = this->request_message.find("\r\n");
		if (cr_lf_pos == str_::npos)
		{
			this->status_code = 400;
			return END;
		}
		chunked_str = this->request_message.substr(0, cr_lf_pos);
		this->request_message = this->request_message.substr(cr_lf_pos + 2);
		if (this->chunked_turn == NUM_TURN)
		{
			this->chunked_size = request_utils::hexStrToLL(chunked_str, over_flow);
			if (over_flow || this->content_length.getValue() < this->chunked_size)
			{
				this->status_code = 400;
				return END;
			}
			if (this->chunked_size == 0)
			{
				return END;
			}
			this->chunked_turn == STR_TURN;
		}
		else if (this->chunked_turn == STR_TURN)
		{
			if (this->chunked_size != chunked_str.size())
			{
				this->status_code = 400;
				return END;
			}
			this->content_length.setValue(this->content_length.getValue() - this->chunked_size);
			this->entity_body += chunked_str;
			this->chunked_turn == NUM_TURN;
		}
	}
	if (this->content_length.getValue() == 0)
		this->status_code = 400;
	return END;
}

/* 
	chunkedでない場合、entity bodyは、\r\nでパースせず
	単純にcontent-length で指定された文字数分そのまま受け取る
*/

bool Request::storeEntityBodyContentLength()
{
	this->entity_body += this->request_message.substr(0, this->content_length.getValue());
	this->request_message = this->request_message.substr(this->content_length.getValue());
	
	if (this->entity_body.size() < this->content_length.getValue())
	{
		this->content_length.setValue(this->content_length.getValue() - this->entity_body.size());
		return CONTINUE;
	}
	return END;
}

bool	Request::parseRequestEntityBody()
{
	if (this->content_length.getStatus() != NOT_SET)
	{
		return Request::storeEntityBodyContentLength();
	}
	else if (this->transfer_encoding.getStatus() != NOT_SET)
	{
		return Request::storeEntityBodyChunked();
	}
}