#include	"../../includes/Config.hpp"


Config::Config(std::string file_name) : _file_name(file_name)
{
	;
}

Config::~Config()
{
	;
}

void	Config::_isValidFileFormat(std::string file_name)
{
	if (file_name.find(".conf") == std::string::npos)
		utils::exitWithError("format error: config file in wrong format");
	if (file_name.size() - file_name.rfind(".conf") != CONF_EXPANSION_LEN)
	{
		utils::exitWithError("format error: config file in wrong format");
		std::exit(EXIT_FAILURE);
	}
}

void	Config::_trimComments(std::string& str)
{
	size_t	comment_pos;

	comment_pos = str.find('#');
	if (comment_pos != std::string::npos)
		str = str.substr(0, comment_pos);
}

void	Config::_trimSpaces(std::string& str)
{
	size_t	start, end;

	start = str.find_first_not_of(SPACES);
	if (start == std::string::npos) {
		str = EMPTY_STRING;
		return ;
	}
	end = str.find_last_not_of(SPACES);
	str = str.substr(start, end - start + 1);
}

std::string	Config::_getPushBackLine(std::string& str)
{
	size_t		bracket_pos;
	std::string	line;

	bracket_pos = str.find_first_of(BRACKETS);
	if (bracket_pos == 0) {
		line = str[bracket_pos];
		str = str.substr(bracket_pos + 1, str.length() - bracket_pos - 1);
	} else if (bracket_pos != std::string::npos) {
		line = str.substr(0, bracket_pos);
		str = str.substr(bracket_pos, str.length() - bracket_pos);
	} else {
		line = str;
		str = EMPTY_STRING;
	}
	return (line);
}

void	Config::_checkValidSemicolon(size_t line_pos, std::string& str)
{
	size_t	semicolomn_pos;

	semicolomn_pos = str.find(';');
	if (semicolomn_pos != std::string::npos && semicolomn_pos + 1 != str.length()) {
		std::cerr << "syntax error: invlid placement of \';\' in line " << line_pos << std::endl;
		exit(EXIT_FAILURE);
	}
	if (str.compare(";") == 0)
		str = EMPTY_STRING;
}

void	Config::readConfigFile(void)
{
	std::ifstream	conf_if;
	std::string		tmp, keep = EMPTY_STRING;
	size_t			line_pos = 0;

	_isValidFileFormat(this->_file_name);
	conf_if.open((this->_file_name).c_str());
	if (!conf_if.is_open() || !conf_if)
		utils::exitWithError("open error: failed to open config file");
	while (true) {
		if (!keep.compare(EMPTY_STRING)) {
			if (!std::getline(conf_if, keep))
				break ;
			line_pos++;
			_trimComments(keep);
		}
		tmp = _getPushBackLine(keep);
		_trimSpaces(tmp);
		_checkValidSemicolon(line_pos, tmp);
		if (tmp.length() != 0)
			this->_config_lines.push_back(tmp);
	}
	conf_if.close();
}

#include <algorithm>

void	Config::_getValidPorts(void)
{
	size_t	size = this->_servers.size();

	for (size_t i = 0; i < size; i++) {
		int	port = this->_servers[i].listen_port.getValue();
		if (std::find(this->_ports.begin(), this->_ports.end(), port) == this->_ports.end())
			this->_ports.push_back(port);
	}
}

void	Config::printConfig(void)
{
	for (size_t i = 0; i < this->_config_lines.size(); ++i) {
		std::cout << this->_config_lines.at(i) << std::endl;
	}
}

void	Config::parseConfig(void)
{
	this->_line_itr = this->_config_lines.begin();
	this->_end_line_itr = this->_config_lines.end();
	if (this->_line_itr == this->_end_line_itr)
		utils::exitWithError("syntax error: found no information in config file");
	while (true) {
		Server	server;
		if ((*this->_line_itr).compare("server") != 0)
			utils::exitWithError("syntax error: information other than server set");
		this->_line_itr++;
		if (this->_line_itr == this->_end_line_itr || (*this->_line_itr).compare("{") != 0)
			utils::exitWithError("syntax error: found no \'{\' for the server");
		this->_line_itr++;
		if (this->_line_itr == this->_end_line_itr)
			utils::exitWithError("syntax error: found no \'}\' to close the server bracket");
		server.parseServer(this->_line_itr, this->_end_line_itr);
		this->_servers.push_back(server);
		this->_line_itr++;
		if (this->_line_itr == this->_end_line_itr)
			break ;
	}
	this->_getValidPorts();
}

std::vector<Server>	Config::getServers(void) const
{
	return (this->_servers);
}

std::vector<int>	Config::getPorts(void) const
{
	return (this->_ports);
}
