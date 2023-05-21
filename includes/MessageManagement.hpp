
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
#define deq_res_ std::deque<Method>
#define vec_sever_ std::vector<Server>

class Request;

typedef struct s_response_message
{
	bool	connection_flg;
	str_	response_message;
}	t_response_message;

class MessageManagement: public Request, public Response 
{
	private:
		
	public:
		Method	*method_p;
		std::deque<t_response_message>	deq_response_message;

		MessageManagement();
		MessageManagement(const MessageManagement &t);
		MessageManagement &operator=(const MessageManagement &rhs);
		~MessageManagement();

		t_response_message	makeResponseMessage(int accepted_socket, const vec_sever_ &servers);
		Server				searchServerWithMatchingPortAndHost(int accepted_socket, const vec_sever_ &servers);
		int 				storeMethodToDeq();

};

std::ostream& operator<<(std::ostream &ostrm, const MessageManagement &tra);

#endif
