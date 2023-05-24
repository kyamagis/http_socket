#include	"../../includes/Server.hpp"

Server::Server() : listen_host("127.0.0.1"), listen_port(80), root("./contents/"), client_max_body_size(1000000)
{
	;
}

Server::~Server()
{
	;
}

void	Server::_parseListen(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->listen_host.getStatus() == SET || this->listen_port.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol listen in server");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for listen");
	vec_str_	host_and_ports = utils::splitStr(split_str[1], ":");
	if (host_and_ports.size() != 2)
		utils::exitWithError("syntax error: invalid format set for listen");
	this->listen_host.setValue(host_and_ports[0]);
	try
	{
		this->listen_port.setValue(parse_utils::strToDigitOverflow<int>(host_and_ports[1], PORT_MIN, PORT_MAX));
	}
	catch(const std::exception& e)
	{
		utils::exitWithError("syntax error: " + str_(e.what()) + " in listen");
	}
}

void	Server::_parseRoot(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->root.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol root in server");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for root");
	this->root.setValue(split_str[1]);
}

void	Server::_parseServerName(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->server_name.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol server_name in server");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for server_name");
	this->server_name.setValue(split_str[1]);
}

void	Server::_parseClientMaxBodySize(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->client_max_body_size.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol client_max_body_size in server");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for max_body_size");
	try
	{
		this->client_max_body_size.setValue(parse_utils::strToDigitOverflow<size_t>(split_str[1], BODY_SIZE_MIN, BODY_SIZE_MAX));
	}
	catch(const std::exception& e)
	{
		utils::exitWithError("syntax error: " + str_(e.what()) + " in client max body size");
	}
}

void	Server::_parseErrorPage(vec_str_& split_str)
{
	size_t			size = split_str.size();
	t_error_page	error_page;

	if (size != 3)
		utils::exitWithError("syntax error: invalid number of arguments for error_page");
	try
	{
		error_page.error_code.setValue(parse_utils::strToDigitOverflow<int>(split_str[1], 100, 599));
	}
	catch(const std::exception& e)
	{
		utils::exitWithError("syntax error: " + str_(e.what()) + " in error_page");
	}
	error_page.error_uri.setValue(split_str[2]);
	this->error_pages.setValue(error_page);
}

void	Server::_getLocation(vec_str_iter_& line_itr, vec_str_iter_& end_line_itr, vec_str_& split_str)
{
	Location	location;

	if (split_str.size() != 2)
		utils::exitWithError("syntax error: invalid number of arguments for location");
	location.path = split_str[1];
	line_itr++;
	if (line_itr == end_line_itr)
		utils::exitWithError("syntax error: found no information for the location");
	if ((*line_itr).compare("{") != 0)
		utils::exitWithError("syntax error: found no \'{\' for the location");
	line_itr++;
	if (line_itr == end_line_itr)
		utils::exitWithError("syntax error: found no \'}\' to close the location bracket");
	location.parseLocation(line_itr, end_line_itr);
	this->locations.setValue(location);
	line_itr++;
	if (line_itr == end_line_itr)
		utils::exitWithError("syntax error: found no \'}\' to close the server bracket");
}

void	Server::parseServer(vec_str_iter_& line_itr, vec_str_iter_& end_line_itr)
{
	vec_str_	split_str;

	while (true) {
		if ((*line_itr).compare("{") == 0)
			utils::exitWithError("syntax error: found no information about the matching brackets");
		else if ((*line_itr).compare("}") == 0)
			break ;
		split_str = utils::splitStr(*line_itr, SPACES);
		if (split_str.size() == 0)
			utils::exitWithError("error occurred while reading context");
		if (split_str[0].compare("location") == 0)
			_getLocation(line_itr, end_line_itr, split_str);
		else {
			if (this->locations.getStatus() == SET)
				utils::exitWithError("validation error: information other than location set after first occurrence of location");
			parse_utils::deleteSemicolomn(split_str);
			if (split_str.size() == 0)
				utils::exitWithError("error occurred while reading context");
			if (split_str[0].compare("listen") == 0)
				_parseListen(split_str);
			else if (split_str[0].compare("root") == 0)
				_parseRoot(split_str);
			else if (split_str[0].compare("server_name") == 0)
				_parseServerName(split_str);
			else if (split_str[0].compare("client_max_body_size") == 0)
				_parseClientMaxBodySize(split_str);
			else if (split_str[0].compare("error_page") == 0)
				_parseErrorPage(split_str);
			else
				utils::exitWithError("syntax error: invalid configuration content for the server");
			line_itr++;
			if (line_itr == end_line_itr)
				utils::exitWithError("syntax error: found no \'}\' to close the server bracket");
		}
	}
}

std::ostream&	operator<<(std::ostream& os, const Server& obj)
{
	os << "<---------- Server information ---------->" << std::endl;
	os << "listen_host: " << obj.listen_host << std::endl;
	os << "listen_port: " << obj.listen_port << std::endl;
	os << "root: " << obj.root << std::endl;
	os << "server_name: " << obj.server_name << std::endl;
	os << "client_max_body_size: " << obj.client_max_body_size << std::endl;
	if (obj.error_pages.getStatus() != NOT_SET) {
		os << "error_pages: " << ((obj.error_pages.getStatus() == DEFAULT_SET) ? "(default set)" : "(set)") << std::endl;
		std::vector<t_error_page> vector = obj.error_pages.getValue();
		size_t	size = vector.size();
		for (size_t i = 0; i < size; i++) {
			os << "[" << i + 1 << "] " << vector[i].error_code << ", " << vector[i].error_uri << std::endl;
		}
	} else
		os << "error_pages: (not set)" << std::endl;
	os << "locations: " << obj.locations;
	return (os);
}
