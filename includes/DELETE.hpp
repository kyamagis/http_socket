#ifndef DELETE_HPP
# define DELETE_HPP

# include	"./Method.hpp"

class DELETE : public Method
{
	private:
		int	_joinIndex(str_ &contents_path);
		int	_deleteFileOrDirectory(const str_ &contents_path);
		int	_dealWithIndexAndAutoindex(str_ &contents_path);

	public:
		DELETE(const Request& r);
		virtual	~DELETE();

		virtual int	exeMethod(const Server& server);
		virtual int	endCGI();

		
};

#endif
