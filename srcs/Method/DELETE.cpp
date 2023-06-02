#include	"../../includes/DELETE.hpp"

DELETE::DELETE(const Request& r) : Method(r)
{
}

DELETE::~DELETE() {}

int	DELETE::endCGI()
{
	return 0;
}

int	DELETE::_joinIndex(str_ &contents_path)
{
	if(Method::joinIndex(contents_path) == 200)
		return 405;
	return 200;
}

#define NOT_FOUND -1

int	DELETE::_deleteFileOrDirectory(const str_ &contents_path)
{
	if (access(contents_path.c_str(), F_OK) == NOT_FOUND)
	{
		return 404;
	}
	if (access(contents_path.c_str(), R_OK) == NOT_FOUND)
	{
		return 403;
	}
	if (std::remove(method_utils::eraseHeadDot(contents_path).c_str()) == -1)// std::remove の機能的にディレクトリは削除できない
	{
		return 403;
	}
	return 200;
}

int	DELETE::_dealWithIndexAndAutoindex(str_ &contents_path)
{
	if (method_utils::isAtStrLast(this->uri, "/") == false)
		return CONTINUE;
	int	status_code = 0;
	if (this->uri == this->_location.path.getValue())
		status_code = Method::joinIndex(contents_path);
	else if (this->_location.autoindex.getValue() == true)
		return 403;
	if (status_code != 200 && this->_location.autoindex.getValue() == true)
		return 403;
	else if (status_code != 200)
		return status_code;
	return CONTINUE;
}

int	DELETE::exeMethod(const Server& server, int max_descripotor)
{
	(void)max_descripotor;
	int	status_code = Method::handleLocation(server);
	if (status_code == 301)//DELETEでリダイレクトは、400として返す.
		return 400;
	if (status_code != 200)
		return status_code;
	if (this->_location.method_delete.getValue() == false)
		return 405;
	str_	contents_path = Method::makeContentsPath(server);
	// status_code = DELETE::_dealWithIndexAndAutoindex(contents_path); // inedx, autoindexのファイルを削除したい場合、コメントアウトを外す
	// if (status_code != CONTINUE)
	// {
	// 	return status_code;
	// }
	return DELETE::_deleteFileOrDirectory(contents_path);
}
