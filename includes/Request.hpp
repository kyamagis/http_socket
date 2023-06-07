
#ifndef Request_hpp
# define Request_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"
#include "./request_utils.hpp"
#include "./RValue.hpp"
#include "./Server.hpp"

#include <arpa/inet.h>

#define vec_sever_ std::vector<Server>

#define CONNECTION_CLOSE 0
#define CONNECTION_KEEP_ALIVE 1

#define END	0
#define CONTINUE 1

#define NUM_TURN 0
#define STR_TURN 1

class Request
{
	private:

		int			_storeChunkedStr();
		int			_gainChunkSize();
		bool		_storeEntityBodyChunked();
		bool		_storeEntityBodyContentLength();
		bool		_parseRequestEntityBody();
		
		void		_parseRequestLine(const str_ &request_line);
		void		_parseConnection(const vec_str_ &vec_split_a_header);
		void		_parseContentLength(const vec_str_ &vec_split_a_header);
		void		_parseTransferEncoding(const vec_str_ &vec_split_a_header);
		int			_parseMime(const str_ &lower_str, const vec_str_ &vec_split_a_header);
		void		_parseContentType(const vec_str_ &vec_split_a_header);
		void		_searchServer(const vec_sever_ &servers);
		void		_parseHost(const str_ &host_header, const vec_sever_ &servers);
		void		_parseHeaders(const vec_str_ &request_headers);
		vec_str_	_spliteRequestLineAndHeader(size_t entity_body_pos);

		int			_setLocation(const Server& server);
		bool		_parseRequestLineAndHeaders(size_t entity_body_pos, const vec_sever_ &servers);
		
	public:

		enum e_request_phase	request_phase;
		int						accepted_socket;
		uint16_t				accepted_socket_port;
		Server					server;
		Location				location;
	
		int			status_code;
		str_		request_message;
		str_		method;
		str_		uri;
		str_ 		version;

		RValue<str_>	host;
		int				port;

		RValue<str_>			content_type;
		RValue<unsigned int>	content_length;
		RValue<str_>			transfer_encoding;
		RValue<bool>			connection;
		str_					request_entity_body;

		bool	chunked_turn; //次回に来るchunkに期待する物のフラグ
		size_t	chunked_size; //

		Request();
		Request(const Request &t);
		Request &operator=(const Request &rhs);
		~Request();

		void	initResponseClass();

		bool	parseRequstMessage(const vec_sever_ &servers);


};

#endif
