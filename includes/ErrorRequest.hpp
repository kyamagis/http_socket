#ifndef ErrorRequest_HPP
# define ErrorRequest_HPP

# include	"./Method.hpp"
# include	"./Config.hpp"

class ErrorRequest : public Method
{
	private:
		int _status_code;

	public:
		ErrorRequest(const Request& r, int status_code);
		virtual	~ErrorRequest();

		virtual int	parseHeaders();
		virtual int	parseRequestMessage();
		virtual int	exeMethod(const Server& server);
};

#endif
