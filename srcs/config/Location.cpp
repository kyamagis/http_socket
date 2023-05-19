#include	"Location.hpp"

Location::Location() : 
	method_get(false), 
	method_post(false), 
	method_delete(false), 
	indexes(vec_str_(1, "index.html")), 
	autoindex(false), 
	client_max_body_size(1000000)
{
	;
}

Location::~Location()
{
	;
}

void	Location::_parseMethod(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (size < 2)
		utils::exitWithError("syntax error: invalid number of arguments for method");
	if (size == 2 && split_str[1].compare("all") == 0) {
		this->method_get.setValue(true);
		this->method_post.setValue(true);
		this->method_delete.setValue(true);
		return ;
	}
	for (size_t i = 1; i < size; i++) {
		if (split_str[i].compare("GET") == 0) {
			if (this->method_get.getValue())
				utils::exitWithError("syntax error: duplicate symbol GET in method");
			this->method_get.setValue(true);
		} else if (split_str[i].compare("POST") == 0) {
			if (this->method_post.getValue())
				utils::exitWithError("syntax error: duplicate symbol POST in method");
			this->method_post.setValue(true);
		} else if (split_str[i].compare("DELETE") == 0) {
			if (this->method_delete.getValue())
				utils::exitWithError("syntax error: duplicate symbol DELETE in method");
			this->method_delete.setValue(true);
		} else
			utils::exitWithError("syntax error: invalid value set for method");
	}
}

void	Location::_parseRoot(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->root.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol root in location");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for root");
	this->root.setValue(split_str[1]);
}

void	Location::_parseIndex(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (size < 2)
		utils::exitWithError("syntax error: invalid number of arguments for index");
	for (size_t i = 1; i < size; i++)
		this->indexes.setValue(split_str[i]);
}

void	Location::_parseAutoindex(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->autoindex.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol autoindex in location");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for autoindex");
	if (split_str[1].compare("on") == 0)
		this->autoindex.setValue(true);
	else if (split_str[1].compare("off") == 0)
		this->autoindex.setValue(false);
	else
		utils::exitWithError("syntax error: invalid value set for autoindex");
}

void	Location::_parseClientMaxBodySize(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->client_max_body_size.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol client_max_body_size in location");
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

void	Location::_parseCgiPath(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->cgi_path.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol cgi_path in location");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for cgi_path");
	this->cgi_path.setValue(split_str[1]);
}

void	Location::_parseUploadPath(vec_str_& split_str)
{
	size_t	size = split_str.size();

	if (this->upload_path.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol upload_path in location");
	if (size != 2)
		utils::exitWithError("syntax error: invalid number of arguments for upload_path");
	this->upload_path.setValue(split_str[1]);
}

void	Location::_parseReturn(vec_str_& split_str)
{
	size_t		size = split_str.size();
	t_return	return_info;

	if (this->return_info.getStatus() == SET)
		utils::exitWithError("syntax error: duplicate symbol return_info in location");
	if (size < 2 || 3 < size)
		utils::exitWithError("syntax error: invalid number of arguments for return");
	if (split_str[1].find_first_not_of(DIGITS) != str_::npos)
		utils::exitWithError("syntax error: non numeric parameter set for return");
	try
	{
		return_info.return_status.setValue(parse_utils::strToDigitOverflow<int>(split_str[1], ERROR_PAGE_MIN, ERROR_PAGE_MAX));
	}
	catch(const std::exception& e)
	{
		utils::exitWithError("syntax error: " + str_(e.what()) + " in return");
	}
	if (size == 3)
		return_info.return_uri.setValue(split_str[2]);
	if (return_info.return_status.getValue() != 301)
		utils::exitWithError("syntax error: cannot set value other than 301 in return");
	this->return_info.setValue(return_info);
}

void	Location::_checkValidLocation(void)
{
	if (this->return_info.getStatus() != NOT_SET) {
		if (this->path.getStatus() == SET || this->method_get.getStatus() == SET || this->method_post.getStatus() == SET
			|| this->method_delete.getStatus() == SET || this->root.getStatus() == SET || this->indexes.getStatus() == SET
			|| this->autoindex.getStatus() == SET || this->client_max_body_size.getStatus() == SET || this->cgi_path.getStatus() == SET
			|| this->upload_path.getStatus() == SET)
			utils::exitWithError("validation error: information other than return set in location");
	} else if (!this->method_get.getValue() && !this->method_post.getValue() && !this->method_delete.getValue()) {
		utils::exitWithError("validation error: no methods specified in location");
	} else if (this->root.getStatus() == NOT_SET)
		utils::exitWithError("validation error: root not set for location");
}

void	Location::parseLocation(vec_str_iter_& line_itr, vec_str_iter_& end_line_itr)
{
	vec_str_	split_str;

	while (true) {
		if ((*line_itr).compare("{") == 0)
			utils::exitWithError("syntax error: found no information about the matching brackets");
		else if ((*line_itr).compare("}") == 0)
			break ;
		split_str = utils::splitStr(*line_itr, SPACES);
		if (split_str.size() == 0)
			utils::exitWithError("error occurred in reading context");
		parse_utils::deleteSemicolomn(split_str);
		line_itr++;
		if (line_itr == end_line_itr)
			utils::exitWithError("syntax error: found no \'}\' to close the location bracket");
		if (split_str[0].compare("method") == 0)
			_parseMethod(split_str);
		else if (split_str[0].compare("root") == 0)
			_parseRoot(split_str);
		else if (split_str[0].compare("index") == 0)
			_parseIndex(split_str);
		else if (split_str[0].compare("autoindex") == 0)
			_parseAutoindex(split_str);
		else if (split_str[0].compare("client_max_body_size") == 0)
			_parseClientMaxBodySize(split_str);
		else if (split_str[0].compare("cgi_path") == 0)
			_parseCgiPath(split_str);
		else if (split_str[0].compare("upload_path") == 0)
			_parseUploadPath(split_str);
		else if (split_str[0].compare("return") == 0)
			_parseReturn(split_str);
		else
			utils::exitWithError("syntax error: invalid configuration content for the location");
	}
	_checkValidLocation();
}

std::ostream&	operator<<(std::ostream& os, const Location& obj)
{
	os << "<------- Location information ------->" << std::endl;
	os << "path: " << obj.path << std::endl;
	os << "method_get: " << obj.method_get << std::endl;
	os << "method_post: " << obj.method_post << std::endl;
	os << "method_delete: " << obj.method_delete << std::endl;
	os << "root: " << obj.root << std::endl;
	os << "indexes: " << obj.indexes << std::endl;
	os << "autoindex: " << obj.autoindex << std::endl;
	os << "client_max_body_size: " << obj.client_max_body_size << std::endl;
	os << "cgi_path: " << obj.cgi_path << std::endl;
	os << "upload_path: " << obj.upload_path << std::endl;
	if (obj.return_info.getStatus() != NOT_SET) {
		t_return return_info = obj.return_info.getValue();
		os << "return_info: " << return_info.return_status << ", " << return_info.return_uri;
	} else
		os << "return_info: (not set)";
	return (os);
}
