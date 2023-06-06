#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include	<iostream>
# include	"Value.hpp"
# include	"Request.hpp"
# include	"GET.hpp"
# include	"POST.hpp"
# include	"DELETE.hpp"
# include	"Server.hpp"
# include	"Location.hpp"

typedef	std::map<int, str_>			map_status_ ;
typedef	std::map<int, str_>			map_wday_ ;
typedef	std::map<int, str_>			map_month_ ;
typedef	std::vector<e_error_page>	vec_e_page_;

class Response
{
	private:
		str_		_getGMT();
		str_		_searchErrorPage(const int status_code, const Server &server);
		str_		_commonResponseMessage(const int status_code, const Server &server);

	public:
		Response();

		map_status_	map_status;
		map_wday_	map_wday;
		map_month_	map_month;

		str_		redirectionResponseMessage(const Value<str_> &response_redirect_uri, const Server &server);
		str_		errorResponseMessage(const int status_code, const Server &server);
		str_		okResponseMessage(const int status_code, 
										Value<str_> response_entity_body,
										str_ content_type,
										RValue<bool> connect,
										const Server &server);
};

#endif
