#include	"../../includes/Response.hpp"
#include	"../../includes/response_utils.hpp"
#include	<ctime>

Response::Response():
			map_status(response_utils::setMapStatus()),
			map_wday(response_utils::setMapWday()),
			map_month(response_utils::setMapMonth()) 
{
	;
}

str_	Response::_getGMT()
{
	std::time_t	now = std::time(NULL);
	if (now == -1)
	{
		return	"std::time() fail";
	}
	std::tm*	gmt = std::gmtime(&now);
	if (gmt == NULL)
	{
		return "std::gmtime() fail";
	}

	str_	date;

	date = this->map_wday[gmt->tm_wday] + ", "
			+ utils::intToStr(gmt->tm_mday) + " " + this->map_month[gmt->tm_mon] + " "
			+ utils::intToStr(gmt->tm_year + 1900) + ' '
			+ utils::intToStr(gmt->tm_hour) + ':' + utils::intToStr(gmt->tm_min) + ':'
			+ utils::intToStr(gmt->tm_sec) + " GMT\r\n";
	return date;
}

str_	Response::_searchErrorPage(const int status_code, const Server &server)
{
	vec_e_page_ vec_e_page = server.error_pages.getValue();
	size_t		size = vec_e_page.size();
	str_		response_message = "\r\n";

	for (size_t i = 0; i < size; i++)
	{
		if (vec_e_page[i].error_code.getValue() == status_code)
		{
			response_message = response_utils::addTypeLengthBody(vec_e_page[i].error_uri.getValue());
			break ;
		}
	}
	return response_message;
}

// str_	Response::addAllowHeader()
// {
// 	str_	response_messagest = "Allow: ";

// 	if (this->_server.locations[0])
// }

str_	Response::_commonResponseMessage(const int status_code, const Server &server)
{
	str_	response_message;

	response_message = "HTTP/1.1 " + this->map_status[status_code];
	response_message += "Date: " + Response::_getGMT();
	response_message += "Server: " + server.server_name.getValue() + "\r\n";
	response_message += "Connection: close\r\n";
	return (response_message);
}

#define	IS_FOUND_REDIRECTION_URI response_redirect_uri.getStatus()
#define REDIRECTION_URI response_redirect_uri.getValue()

str_	Response::redirectionResponseMessage(const Value<str_> &response_redirect_uri, RValue<bool> connect, const Server &server)
{
	str_	response_message = Response::_commonResponseMessage(301, server);

	if (IS_FOUND_REDIRECTION_URI != NOT_SET)
	{
		response_message += "Location: " + REDIRECTION_URI + "\r\n";
	}
	return response_message + "\r\n";
}

str_	Response::errorResponseMessage(const int status_code, RValue<bool> connect, const Server &server)
{
	str_	response_message = Response::_commonResponseMessage(status_code, server);

	if (server.error_pages.getStatus() != NOT_SET)
	{
		return response_message + Response::_searchErrorPage(status_code, server);
	}
	return response_message + "\r\n";
}

str_		Response::okResponseMessage(const int status_code, 
										Value<str_> response_entity_body,
										str_ content_type,
										RValue<bool> connect,
										const Server &server)
{
	str_	response_message = Response::_commonResponseMessage(status_code, server);

	if (response_entity_body.getStatus() != NOT_SET)
	{
		str_	str_response_entity_body = response_entity_body.getValue();

		response_message += "Content-Length: " + utils::to_string(str_response_entity_body.size()) + "\r\n";
		response_message += "Content-Type: " + content_type + "\r\n";
		response_message += "\r\n";
		return response_message + str_response_entity_body;
	}
	return response_message + "\r\n";
}
