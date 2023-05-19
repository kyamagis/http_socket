#include "../../includes/GET.hpp"

GET::GET(const Request& r) : Method(r)
{
}

GET::~GET() {}

///////////////////////////////////////////////////////////////////////////////////////////////////////

#define NOT_FOUND -1

int GET::_readFileContents(const str_ &contents_path)
{
	std::ifstream ifs;

	if (request_utils::isAtStrLast(contents_path, "/"))
		return 404;
	if (access(contents_path.c_str(), F_OK) == NOT_FOUND)
		return 404;
	if (access(contents_path.c_str(), R_OK) == NOT_FOUND)
		return 403;
	ifs.open(contents_path.c_str());
	if (!ifs || !ifs.is_open())
		return 404;
	str_ tmp((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	this->_response_entity_body.setValue(tmp);
	ifs.close();
	this->_response_content_type = request_utils::discriminateExtension(contents_path);
	return 200;
}

#define CGI_PATH this->_location.cgi_path.getValue()

int GET::_exeCGI(const str_ &contents_path)
{
	CGI cgi(CGI_PATH, Method::setEnv(), contents_path, this->_request_entity_body);

	int status_code = cgi.exeCGI();
	if (status_code != 200)
		return status_code;
	this->_response_entity_body.setValue(cgi.getResult());
	this->_response_content_type = "text/plane";
	return status_code;
}

int GET::_exeAutoindex(const str_ &directory_path)
{
	DIR *dir;
	struct dirent *dirent;
	str_ str;
	str_ file;
	str_ location_path = this->_location.path.getValue();

	str += "<html>\n";
	str += "<head><title>Index of " + location_path + "</title></head>\n";
	str += "<body>\n";
	str += "<h1>Index of " + location_path + "</h1><hr><pre>\n";
	dir = opendir(directory_path.c_str());
	if (!dir)
		return 404;
	while (true)
	{
		dirent = readdir(dir);
		if (!dirent)
			break;
		file = str_(dirent->d_name);
		if (file == ".")
			continue;
		if (dirent->d_type == DT_DIR)
			file += "/";
		str += "<a href=\"" + file + "\">" + file + "</a>\n";
	}
	closedir(dir);
	str += "</pre><hr></body>\n";
	str += "</html>\n";
	this->_response_entity_body.setValue(str);
	return 200;
}

int GET::_dealWithIndexAndAutoindex(str_ &contents_path)
{
	if (request_utils::isAtStrLast(this->uri, "/") == false)
		return CONTINUE;
	int status_code = 0;
	if (this->uri == this->_location.path.getValue())
		status_code = Method::joinIndex(contents_path);
	else if (this->_location.autoindex.getValue() == true)
		return GET::_exeAutoindex(contents_path);
	if (status_code != 200 && this->_location.autoindex.getValue() == true)
		return GET::_exeAutoindex(contents_path);
	else if (status_code != 200)
		return status_code;
	return CONTINUE;
}

int GET::exeMethod(const Server &server)
{
	int status_code;
	str_ contents_path;

	status_code = Method::handleLocation(server);
	if (status_code != 200)
		return status_code;
	if (this->_location.method_get.getValue() == false)
		return 405;
	contents_path = Method::makeContentsPath(server);

	status_code = GET::_dealWithIndexAndAutoindex(contents_path);
	if (status_code != CONTINUE)
	{
		return status_code;
	}
	if (this->_location.cgi_path.getStatus() != NOT_SET)
		return GET::_exeCGI(contents_path);
	return GET::_readFileContents(contents_path);
}