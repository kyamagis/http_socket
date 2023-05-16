
#include "./ResponseMessage.hpp"

ResponseMessage::ResponseMessage(): response_phase(RECV_REQUEST)
{

}

ResponseMessage::~ResponseMessage()
{

}

ResponseMessage::ResponseMessage(const str_ &str): response_message(str)
{

}

ResponseMessage::ResponseMessage(const ResponseMessage &rm)
{
	*this = rm;
}

ResponseMessage &ResponseMessage::operator=(const ResponseMessage &rhs)
{
	if (this != &rhs)
	{
		this->response_phase   = rhs.response_phase;
		this->response_message = rhs.response_message;
	}
	return *this;
}
		