
#ifndef MessageManagement_hpp
# define MessageManagement_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"
#include "./GET.hpp"
#include "./POST.hpp"
#include "./DELETE.hpp"
#include "./ErrorRequest.hpp"
#include "./Request.hpp"
#include "./IOMultiplexing.hpp"
#include "./Response.hpp"

#define deq_Method_ std::deque<Method>
#define vec_sever_ std::vector<Server>

class Request;

class MessageManagement: public Request, public Response 
{
	private:
		
	public:
		deq_Method_	deq_method;

		MessageManagement();
		MessageManagement(const MessageManagement &t);
		MessageManagement &operator=(const MessageManagement &rhs);
		~MessageManagement();

		str_	makeResponseMessage(int accepted_socket, const vec_sever_ &servers);
		Server	searchServerWithMatchingPortAndHost(int accepted_socket, const vec_sever_ &servers);
		int 	storeMethodToDeq();

};

std::ostream& operator<<(std::ostream &ostrm, const MessageManagement &tra);

#endif