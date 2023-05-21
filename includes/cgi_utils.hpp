#ifndef cgi_utils_hpp
# define cgi_utils_hpp

#include "./utils.hpp"

namespace cgi_utils
{
	void	closePutError(int fd);
	char	*convertStrToCharP(std::string str);
	void	ft_strlcpy(char *charp_str, str_ str, size_t size);
	char	*newStrDup(str_ str);
	char	**dupVecToCharDoubleP(vec_str_ vec_str, size_t size);
	char	**makeCharDoublePointer(str_ str);
	int		x_kill(pid_t pid);
	int		waitpidAndKill(pid_t pid);
	clock_t	getMicroSec();
	void 	x_dup2(int oldfd, int newfd);
}

#endif
