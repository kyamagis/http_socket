#include "../../includes/POST.hpp"

POST::POST(const Request& r) : Method(r)
{
}


POST::~POST() {}

#define CGI_PATH this->_location.cgi_path.getValue()

/* contents_pathがファイルでもディレクトリでもCGIに渡し実行 */
/* この場合、どこpathにファイルを生成すべきか */

int POST::_dealWithIndex(str_ &contents_path)
{
	if (request_utils::isAtStrLast(this->uri, "/") == false) // this->uriがfileだったとき
		return 200;
	if (this->uri == this->_location.path.getValue())
	{
		return Method::joinIndex(contents_path);
	}
	return 404;
}

int	POST::_startCGI(const str_ &contents_path)
{
	this->_contents_path = contents_path;
	int	status_code = POST::_dealWithIndex(this->_contents_path);

	if (status_code != 200)
	{
		return status_code;
	}
	this->_last_slash_index = contents_path.rfind('/');
	if (this->_last_slash_index == str_::npos)
	{
		return 500; //適当
	}
	this->cgi.setCGI("POST", CGI_PATH, Method::setEnv(contents_path), contents_path, this->request_entity_body);
	this->_contents_path = contents_path;
	return this->cgi.startCGI();
}


#define CGI_CONTENT_TYPE this->cgi.content_type.getValue()

int	POST::endCGI()
{	
	str_	file_name;

	if (this->_location.upload_path.getStatus() == NOT_SET)
	{
		/* contents_path で指定されたfileと同じ階層にCGIで作ったfileを置く */
		str_ contents_directory_path = this->_contents_path.substr(0, this->_last_slash_index + 1); // ./content/index.html -> ./content/
		file_name = request_utils::createUniqueFileName(contents_directory_path, CGI_CONTENT_TYPE);
	}
	else
		file_name = this->_upload_path.getValue();

	return request_utils::makeAndPutFile(cgi.getCGIExecResult(), file_name);
}

void	POST::_makeUploadPath(const Server &server)
{
	if (this->_location.upload_path.getStatus() == NOT_SET)
		return;
	str_	upload_path = request_utils::joinPath(server.root.getValue(), this->_location.upload_path.getValue());
	this->_upload_path.setValue(upload_path);
}

#define CONTENT_TYPE this->content_type.getValue()

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
	POST::_makeUploadPath(server);
	if (this->_location.cgi_path.getStatus() != NOT_SET)
		return POST::_startCGI(contents_path);
	if (request_utils::isAtStrLast(contents_path, "/")) //　contents_path　== ./directory/
	{
		str_ file_name;
		if (this->_location.upload_path.getStatus() == NOT_SET)
			file_name = request_utils::createUniqueFileName(contents_path, CONTENT_TYPE);
		else
			file_name = request_utils::createUniqueFileName(this->_upload_path.getValue(), CONTENT_TYPE);
		return request_utils::makeAndPutFile(this->request_entity_body, file_name);
	}
	return request_utils::overwriteFile(this->request_entity_body, contents_path);
}
