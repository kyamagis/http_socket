
#ifndef Request_hpp
# define Request_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"
#include "./request_utils.hpp"
#include "./Method.hpp"
#include "./RValue.hpp"

#define CONNECTION_CLOSE 0
#define CONNECTION_KEEP_ALIVE 1

#define END	0
#define CONTINUE 1

#define NUM_TURN 0
#define STR_TURN 1

class Request
{
	private:
		
	public:

		enum e_request_phase	request_phase;

		int			status_code;
		str_		request_message;
		str_		method;
		str_		uri;
		str_ 		version;

		RValue<str_>	host;
		int			port;

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

		vec_str_	spliteRequestLineAndHeader(size_t entity_body_pos);
		void		parseRequestLine(const str_ &request_line);
		void		parseHost(const str_ &host_header);
		void		parseHeaders(const vec_str_ &request_headers);
		void		parseContentType(const vec_str_ &vec_split_a_header);
		int			parseMime(const str_ &lower_str, const vec_str_ &vec_split_a_header);
		void		parseContentLength(const vec_str_ &vec_split_a_header);
		void		parseTransferEncoding(const vec_str_ &vec_split_a_header);
		void		parseConnection(const vec_str_ &vec_split_a_header);
		bool		parseRequestLineAndHeaders(size_t entity_body_pos);
		bool		parseRequestEntityBody();
		bool		storeEntityBodyContentLength();
		bool		storeEntityBodyChunked();
		bool		parseRequstMessage();


};

#endif
