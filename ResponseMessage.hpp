#ifndef ResponseMessage_hpp
# define ResponseMessage_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"


class ResponseMessage
{
	private:
		ResponseMessage();

	public:
		enum e_phase	response_phase;
		str_			response_message;

		ResponseMessage(const str_ &str);
		ResponseMessage(const ResponseMessage &rm);
		ResponseMessage &operator=(const ResponseMessage &rhs);
		~ResponseMessage();
		
	

};

#endif