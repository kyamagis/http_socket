
#include "../../includes/request_utils.hpp"

#include <climits>
#include <ctime>
#include <fstream>

static bool isHex(int c)
{
	if (('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'))
	{
		return true;
	}
	return false;
}

static size_t checkHexMax(unsigned long num, int flag, size_t digcount, bool &over_flow)
{
	if (flag == 1 && 19 < digcount)
	{
		over_flow = true;
		return -1;
	}
	if (flag == 1 && ULONG_MAX <= num)
	{
		over_flow = true;
		return -1;
	}
	if (flag == -1)
	{
		over_flow = true;
		return -1;
	}
	return (num * flag);
}

static int hexToNum(int c)
{
	switch (c)
	{
		case 'a':
		case 'A':
			return 10;
		case 'b':
		case 'B':
			return 11;
		case 'c':
		case 'C':
			return 12;
		case 'd':
		case 'D':
			return 13;
		case 'e':
		case 'E':
			return 14;
		case 'f':
		case 'F':
			return 15;
		default:
			return c - '0';
	}
}

namespace request_utils
{
	size_t hexStrToLL(const str_ &str, bool &over_flow)
	{
		size_t 	num = 0;
		int		flag = 1;
		size_t	digcount = 1;
		size_t	i = 0;

		if (!isHex(str[i]))
		{
			over_flow = true;
			return -1;
		}
		while (isHex(str[i]))
		{
			if (num != 0)
				digcount++;
			num = 16 * num + hexToNum(str[i]);
			i++;
		}
		if ((str[i]) || (!str[i] && i == 0))
		{
			over_flow = true;
			return -1;
		}
		return (checkHexMax(num, flag, digcount, over_flow));
	}

	uint16_t	getPortFromAcceptedSocket(int accepted_socket)
	{
		struct sockaddr_in	addr;
		socklen_t			addr_len = sizeof(addr);

		std::memset(&addr, 0, sizeof(sockaddr));
		if (getsockname(accepted_socket, (struct sockaddr *)&addr, &addr_len) == -1)
		{
			utils::putError("getsockname() faile");
			return (ntohs(80));
		}
		return ntohs(addr.sin_port);
	}
}
