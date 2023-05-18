
#ifndef MessageManagement_hpp
# define MessageManagement_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"
#include "./ResponseMessage.hpp"
#include "./Request.hpp"

#define deq_response_message_ std::deque<ResponseMessage>

class Request;

class MessageManagement: public Request 
{
	private:
		
	public:
		enum e_phase			request_phase;
		deq_response_message_	deq_response_message;

		MessageManagement();
		MessageManagement(const MessageManagement &t);
		MessageManagement &operator=(const MessageManagement &rhs);
		~MessageManagement();

};

#endif