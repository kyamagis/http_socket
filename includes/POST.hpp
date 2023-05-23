#ifndef POST_HPP
# define POST_HPP

# include	"./Method.hpp"

class POST : public Method
{
	private:
		int	_dealWithIndexAndAutoindex(str_ &contents_path);
		int	_staertCGI(const str_ &contents_path);

	public:
		POST(const Request& r);

		virtual	~POST();
		virtual int	exeMethod(const Server& server);
		virtual int	endCGI();
};

#endif
