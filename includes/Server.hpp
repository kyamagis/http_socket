#ifndef SERVER_HPP
# define SERVER_HPP

# include	<vector>
# include	<iostream>
# include	"Value.hpp"
# include	"Location.hpp"
# include	"utils.hpp"

# define PORT_MIN 0
# define PORT_MAX 65535

typedef struct e_error_page
{
	Value<int>			error_code;
	Value<std::string>	error_uri;
}	t_error_page;

class Server
{
	private:
		void	_parseListen(vec_str_& split_str);
		void	_parseRoot(vec_str_& split_str);
		void	_parseServerName(vec_str_& split_str);
		void	_parseClientMaxBodySize(vec_str_& split_str);
		void	_parseErrorPage(vec_str_& split_str);
		void	_getLocation(vec_str_::iterator& line_itr, vec_str_::iterator& end_line_itr, vec_str_& split_str);
		void	_checkValidServer(void);

	public:
		Value<std::string>					listen_host;
		Value<int>							listen_port;
		Value<std::string>					root;
		Value<std::string>					server_name;
		Value<size_t>						client_max_body_size;
		Value<std::vector<t_error_page> >	error_pages;
		Value<std::vector<Location> >		locations;
		typedef std::vector<Server>::iterator 		server_itr_;

		Server();
		~Server();
		void		parseServer(vec_str_::iterator& line_itr, vec_str_::iterator& end_line_itr);

		class InvalidPathException : public std::exception
		{
			const char	*what() const throw();
		};
		class NoMatchingLocationException : public std::exception
		{
			const char	*what() const throw();
		};
};

std::ostream&	operator<<(std::ostream& os, const Server& obj);

#endif
