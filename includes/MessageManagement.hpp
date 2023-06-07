
#ifndef MessageManagement_hpp
# define MessageManagement_hpp

#include <iostream>
#include <string>
#include <deque>
#include <cstring>

#include <arpa/inet.h>

#include "./utils.hpp"
#include "./GET.hpp"
#include "./POST.hpp"
#include "./DELETE.hpp"
#include "./ErrorRequest.hpp"
#include "./Request.hpp"
#include "./Response.hpp"
#include "./mm_utils.hpp"

#define deq_Method_ std::deque<Method>
#define deq_res_ std::deque<Method>
#define vec_sever_ std::vector<Server>

class Request;
class Method;

typedef struct s_response_message
{
	bool	connection_flg;
	str_	response_message;
}	t_response_message;

enum e_attribution
{
	NOT_CGI,
	DURING_CGI,
	WRITE_CGI,
	READ_CGI
};

class MessageManagement: public Request, public Response 
{
	private:
		
	public:
		enum e_attribution attribution;
		Method	*method_p;
		int		parent_fd;

		std::deque<t_response_message>	deq_response_message;

		MessageManagement();
		MessageManagement(const MessageManagement &t);
		MessageManagement &operator=(const MessageManagement &rhs);
		~MessageManagement();

		void	setDefaultServer(const vec_sever_ &servers);
		int		makeResponseMessage(t_response_message	&response_message, int max_descripotor);
		int 	storeMethodToDeq();
		int		readCGIResponse(t_response_message &response_message);
		int		writeCGIRequest(t_response_message &response_message);
		bool	parseRequstMessage(const vec_sever_ &servers);
};

std::ostream& operator<<(std::ostream &ostrm, const MessageManagement &tra);

#endif
