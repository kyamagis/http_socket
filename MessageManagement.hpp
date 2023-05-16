
#ifndef MessageManagement_hpp
# define MessageManagement_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"
#include "./ResponseMessage.hpp"

#define deq_response_message_ std::deque<ResponseMessage>

class MessageManagement
{
	private:
		
	public:
		enum e_phase			request_phase;
		str_					request_message;
		deq_response_message_	deq_response_message;

		MessageManagement();
		MessageManagement(const MessageManagement &t);
		MessageManagement &operator=(const MessageManagement &rhs);
		~MessageManagement();

};

#endif