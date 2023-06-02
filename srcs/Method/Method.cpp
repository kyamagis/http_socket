
#include "../../includes/Method.hpp"

Method::Method()
{

}

Method::~Method()
{

}

Method::Method(const Request &r):
	_location(r.location),
	method(r.method),
	uri(r.uri),
	version(r.version),
	host(r.host),
	port(r.port),
	content_type(r.content_type),
	content_length(r.content_length),
	transfer_encoding(r.transfer_encoding),
	connection(r.connection),
	request_entity_body(r.request_entity_body),
	chunked_turn(r.chunked_turn),
	chunked_size(r.chunked_size)
{

}

Method::Method(const Method &rm)
{
	*this = rm;
}

Method &Method::operator=(const Method &rhs)
{
	if (this != &rhs)
	{
		this->response_phase   = rhs.response_phase;

		this->response_message = rhs.response_message;

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


bool	Method::isBackSlashAtPathAndRootLast()
{
	if (!method_utils::isAtStrLast(this->_location.path.getValue(), "/"))
		return false;
	if (this->_location.root.getStatus())
		if (!method_utils::isAtStrLast(this->_location.root.getValue(), "/"))
			return false;
	return true;
}

int	Method::handleLocationRedirect()
{
	if (this->_location.return_info.getStatus() != NOT_SET)
	{
		t_return	redirect = this->_location.return_info.getValue();
		if (redirect.return_uri.getStatus() != NOT_SET)
			this->_response_redirect_uri.setValue(redirect.return_uri.getValue());
		return redirect.return_status.getValue();
	}
	return 200;
}

int	Method::handleLocation(const Server& server)
{
	int	status_code;

	this->server_name = server.server_name.getValue();
	this->request_port = server.listen_port.getValue();
	status_code = Method::handleLocationRedirect();
	if (status_code != 200)
		return status_code;
	return 200;
}

str_	Method::makeContentsPath(const Server& server)
{
	str_	contents_path;

	contents_path = this->uri;
	contents_path.erase(0, this->_location.path.getValue().length());
	contents_path.insert(0, this->_location.root.getValue());
	contents_path = utils::joinPath(server.root.getValue(), contents_path);
	return (contents_path);
}

#define FOUND 0

int	Method::joinIndex(str_& contents_path)
{
	vec_str_	indexes = this->_location.indexes.getValue();
	size_t		size = indexes.size();
	str_		tmp;

	for (size_t i = 0; i < size; i++) {
		tmp = utils::joinPath(contents_path, indexes[i]);
		if (access(tmp.c_str(), R_OK) == FOUND) {
			contents_path = tmp;
			return 200;
		}
	}
	return 404;
}

map_env_	Method::setEnv(const str_ &contents_path)
{
	map_env_	map_env;

	if (this->content_type.getStatus())
		map_env["CONTENT_TYPE"] = this->content_type.getValue();
	if (this->content_length.getStatus())
		map_env["CONTENT_LENGTH"] = utils::to_string(this->content_length.getValue());
	map_env["PATH_INFO"] = contents_path;
	map_env["REQUEST_METHOD"] = this->method;
	map_env["SERVER_NAME"] = this->server_name;
	map_env["SERVER_PORT"] = utils::to_string(this->request_port);
	map_env["SERVER_PROTOCOL"] = this->version;
	map_env["URI"] = this->uri;
	
	map_env["HOST"] = this->host.getValue();
	
	return map_env;
}

int	Method::endCGI()
{
	return 0;
}

Value<str_>	Method::getResponse_entity_body()
{
	return this->_response_entity_body;
}

str_		Method::getContentType()
{
	return this->_response_content_type;
}

Value<str_>		Method::getResponse_redirect_uri()
{
	return this->_response_redirect_uri;
}

Value<str_>		Method::getRequest_entity_body()
{
	return this->_request_entity_body;
}

int	Method::exeMethod(const Server& server, int max_descripotor) 
{
	(void)server;
	(void)max_descripotor;
	return 0;
}
