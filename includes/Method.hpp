#ifndef Method_hpp
# define Method_hpp

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <deque>

#include "./utils.hpp"
#include "./RValue.hpp"
#include "./Request.hpp"
#include "./Config.hpp"
#include "./CGI.hpp"

typedef std::map<std::string, std::string>	map_env_;

class Request;

class Method
{
	protected:
		Method();

		Value<str_>	_request_entity_body;
		Location	_location;
		Value<str_>	_response_redirect_uri;
		Value<str_>	_response_entity_body;
		str_		_response_content_type;
		str_ 		_raw_request;

	public:
		enum e_phase	response_phase;

		str_		response_message;

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

		Method(const Request &r);
		Method(const Method &rm);
		Method &operator=(const Method &rhs);
		~Method();
		
		int		handleLocationRedirect();
		int		handleLocation(const Server& server);
		str_	makeContentsPath(const Server& server);
		int		joinIndex(str_& contents_path);
		bool	isBackSlashAtPathAndRootLast();
		int		setLocation(const Server& server);

		Value<str_>		getResponse_entity_body();
		Value<str_>		getResponse_redirect_uri();
		str_			getContentType();	
		Value<str_>		getRequest_entity_body();
		map_env_		setEnv();

		virtual	int	exeMethod(const Server& server) {(void)server; return 0;};

};

#endif