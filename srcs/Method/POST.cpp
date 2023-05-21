#include "../../includes/POST.hpp"

POST::POST(const Request& r) : Method(r)
{
}


POST::~POST() {}

#define CONTENT_TYPE this->content_type.getValue()
#define PIPE_MAX 65536 // pipeに書き込める最大バイト数.これを超えるとブロッキングが発生する
#define CGI_PATH this->_location.cgi_path.getValue()

/* contents_pathがファイルでもディレクトリでもCGIに渡し実行 */
/* この場合、どこpathにファイルを生成すべきか */

int POST::_exeCGI(const str_ &contents_path)
{
	if (PIPE_MAX <= this->request_entity_body.size()) // いらないかも
		return 403;

	CGI cgi("POST", CGI_PATH, Method::setEnv(), contents_path, this->request_entity_body);

	int status_code = cgi.exeCGI();
	if (status_code != 200)
		return status_code;
	size_t last_slash_index = contents_path.rfind('/');
	if (last_slash_index == str_::npos)
	{
		return 500; //適当
	}
	str_ contents_directory_path = contents_path.substr(0, last_slash_index + 1); // ./content/index.html -> ./content/
	str_ file_name = request_utils::createUniqueFileName(contents_directory_path, CONTENT_TYPE);
	status_code = request_utils::makeAndPutFile(cgi.getResult(), file_name);
	return status_code;
}

int POST::_dealWithIndexAndAutoindex(str_ &contents_path)
{
	if (request_utils::isAtStrLast(this->uri, "/") == false)
		return 200;
	int status_code = 0;
	if (this->uri == this->_location.path.getValue())
	{
		status_code = Method::joinIndex(contents_path);
		if (status_code == 200 && this->_location.cgi_path.getStatus() == NOT_SET) // cgiを使用しないのに、indexファイルをPOSTの対象にする場合は、エラー
		{
			return 403;
		}
	}
	else if (this->_location.autoindex.getValue() == true)
		return 403;
	if (status_code != 200 && this->_location.autoindex.getValue() == true)
		return 403;
	return 200;
}

int POST::exeMethod(const Server &server)
{
	if (this->content_type.getStatus() == NOT_SET)
		return 400;
	if (this->content_length.getStatus() == NOT_SET && this->transfer_encoding.getStatus() == NOT_SET )
		return 400;
	int status_code = Method::handleLocation(server);
	if (status_code == 301) // POSTでリダイレクトは、400として返す.
		return 400;
	if (status_code != 200)
		return status_code;
	if (this->_location.method_post.getValue() == false)
		return 405;
	str_ contents_path = Method::makeContentsPath(server);
	status_code = POST::_dealWithIndexAndAutoindex(contents_path);
	if (status_code != 200)
		return status_code;
	if (this->_location.cgi_path.getStatus() != NOT_SET)
		return POST::_exeCGI(contents_path);
	if (request_utils::isAtStrLast(contents_path, "/")) //　contents_path　== ./directory/
	{
		str_ file_name = request_utils::createUniqueFileName(contents_path, CONTENT_TYPE);
		return request_utils::makeAndPutFile(this->request_entity_body, file_name);
	}
	return request_utils::overwriteFile(this->request_entity_body, contents_path);
}
