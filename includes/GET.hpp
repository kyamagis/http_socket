#ifndef GET_HPP
# define GET_HPP

# include	<dirent.h>
# include	"./Method.hpp"

class GET : public Method
{
	private:
		int		_readFileContents(const str_& contents_path);
		int		_exeCGI(const str_& contents_path);
		int		_exeAutoindex(const str_& directory_path);
		int		_dealWithIndexAndAutoindex(str_ &contents_path);

	public:

		GET(const Request& r);

		virtual	~GET();
		virtual int	exeMethod(const Server& server);
		
};

#endif
