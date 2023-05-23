#ifndef POST_HPP
# define POST_HPP

# include	"./Method.hpp"

class POST : public Method
{
	private:
		str_	_contents_path;
		size_t	_last_slash_index;
		int		_dealWithIndexAndAutoindex(str_ &contents_path);
		int		_startCGI(const str_ &contents_path);

	public:
		POST(const Request& r);

		virtual	~POST();
		virtual int	exeMethod(const Server& server);
		virtual int	endCGI();
};

#endif
