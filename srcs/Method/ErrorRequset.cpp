#include	"../../includes/ErrorRequest.hpp"

ErrorRequest::ErrorRequest(const Request& r, int status_code): Method(r), _status_code(status_code)
{

}

ErrorRequest::~ErrorRequest() {}

int	ErrorRequest::exeMethod(const Server& server)
{

	(void)server;
	return this->_status_code;
}
