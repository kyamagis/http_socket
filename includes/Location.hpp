#ifndef LOCATION_HPP
# define LOCATION_HPP

# include	<vector>
# include	<iostream>
# include	"Value.hpp"
# include	"utils.hpp"

# define ERROR_PAGE_MIN 0
# define ERROR_PAGE_MAX 999
# define BODY_SIZE_MIN 0
# define BODY_SIZE_MAX INT_MAX

typedef struct e_return
{
	Value<int>	return_status;
	Value<str_>	return_uri;
}	t_return;

class Location
{
	private:
		void	_parseMethod(vec_str_& split_str);
		void	_parseRoot(vec_str_& split_str);
		void	_parseIndex(vec_str_& split_str);
		void	_parseAutoindex(vec_str_& split_str);
		void	_parseClientMaxBodySize(vec_str_& split_str);
		void	_parseCgiPath(vec_str_& split_str);
		void	_parseUploadPath(vec_str_& split_str);
		void	_parseReturn(vec_str_& split_str);
		void	_checkValidLocation(void);

	public:
		Value<str_>			path;
		Value<bool>			method_get;
		Value<bool>			method_post;
		Value<bool>			method_delete;
		Value<str_>			root;
		Value<vec_str_ >	indexes;
		Value<bool>			autoindex;
		Value<size_t>		client_max_body_size;
		Value<str_>			cgi_path;
		Value<str_>			upload_path;
		Value<t_return>		return_info;

		Location();
		~Location();
		void	parseLocation(vec_str_iter_& line_itr, vec_str_iter_& end_line_itr);
};

std::ostream&	operator<<(std::ostream& os, const Location& obj);

#endif
