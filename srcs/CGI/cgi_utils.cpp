
#include "./cgi_utils.hpp"

namespace cgi_utils
{
	void closePutError(int fd)
	{
		if (close(fd) == -1)
		{
			std::cerr << "close fail" << std::endl;
		}
	}

	char *convertStrToCharP(std::string str)
	{
		char *str_char;
		size_t i;
		size_t str_length = str.length();

		str_char = new char[str_length + 1];
		for (i = 0; i < str_length; i++)
			str_char[i] = str[i];
		str_char[i] = '\0';
		return (str_char);
	}

	void ft_strlcpy(char *charp_str, str_ str, size_t size)
	{
		size_t i = 0;

		for (; i < size; i++)
		{
			charp_str[i] = str[i];
		}
		charp_str[i] = '\0';
	}

	char *newStrDup(str_ str)
	{
		char *charp_str;

		charp_str = new char[str.size() + 1];
		ft_strlcpy(charp_str, str, str.size());
		return charp_str;
	}

	char **dupVecToCharDoubleP(vec_str_ vec_str, size_t size)
	{
		char **argv;

		argv = new char *[size + 1];
		for (size_t i = 0; i < size; i++)
		{
			argv[i] = const_cast<char *>(newStrDup(vec_str[i].c_str()));
		}
		argv[size] = NULL;
		return argv;
	}

	char **makeCharDoublePointer(str_ str)
	{
		char **argv;
		vec_str_ vec_str = utils::split_Str(str, " ");

		try
		{
			argv = dupVecToCharDoubleP(vec_str, vec_str.size());
		}
		catch (const std::exception &e)
		{
			utils::exitWithError("new fail");
		}
		return argv;
	}

	int x_kill(pid_t pid)
	{
		if (kill(pid, SIGKILL) == -1)
		{
			std::cerr << "kill fail" << std::endl;
			return 500;
		}
		return 508;
	}

	int waitpidAndKill(pid_t pid)
	{
		int wstatus;
		clock_t start_time = getMicroSec();

		if (start_time == CLOCK_FAIL)
		{
			return 500;
		}
		while (waitpid(pid, &wstatus, WNOHANG) == 0) //中断することなく子プロセスをチェック https://www.ibm.com/docs/ja/zos/2.2.0?topic=functions-waitpid-wait-specific-child-process-end
		{
			if (READ_TIME_OUT < getMicroSec() - start_time)
			{
				return x_kill(pid);
			}
		}
		if (!WIFEXITED(wstatus))
		{
			return 500;
		}
		switch (WEXITSTATUS(wstatus))
		{
		case EXIT_SUCCESS:
			return 200;
		default:
			return 500;
		}
		return (200);
	}

	clock_t getMicroSec()
	{
		clock_t t = std::clock(); // 納得していない 定数時間を測るのに適しているとは思えない

		if (t == CLOCK_FAIL)
		{
			return CLOCK_FAIL;
		}
		return (t);
	}

	void x_dup2(int oldfd, int newfd)
	{
		if (dup2(oldfd, newfd) == -1)
		{
			utils::exitWithError("dup2 fail");
		}
	}
}