#ifndef POST_HPP
# define POST_HPP

# include	"./Method.hpp"

class POST : public Method
{
	private:
		str_		_contents_path;
		size_t		_last_slash_index;
		Value<str_>	_upload_path;

		int		_dealWithIndex(str_ &contents_path);
		int		_startCGI(const str_ &contents_path, int max_descripotor);
		void	_makeUploadPath(const Server &server);

	public:
		POST(const Request& r);


		virtual	~POST();
		virtual int	exeMethod(const Server& server, int max_descripotor);
		virtual int	endCGI();
};

#endif
