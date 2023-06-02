
#include "../../includes/mm_utils.hpp"

namespace mm_utils
{
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