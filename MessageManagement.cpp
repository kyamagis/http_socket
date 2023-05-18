#include "./MessageManagement.hpp"

MessageManagement::MessageManagement()
{

}
	
MessageManagement::~MessageManagement()
{

}

MessageManagement::MessageManagement(const MessageManagement &t)
{
	*this = t;
}

MessageManagement &MessageManagement::operator=(const MessageManagement &rhs)
{
	if (this != &rhs)
	{
		this->request_phase        = rhs.request_phase;
		this->request_message      = rhs.request_message;
		this->deq_response_message = rhs.deq_response_message;
	}
	return *this;
}
