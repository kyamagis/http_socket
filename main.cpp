
#include "./IOMultiplexing.hpp"

vec_int_	makeVecPortsFormCharP(int argc, char **argv)
{
	vec_int_	vec_ports;

	for(int i = 0; i < argc; i++)
	{
		vec_ports.push_back(atoi(argv[i]));
	}
	return vec_ports;
}

int	main(int argc, char **argv)
{
    if (argc < 2)
		utils::exitWithPutError("Too few argument");

    signal(SIGPIPE, SIG_IGN);
	vec_int_	vec_port = makeVecPortsFormCharP(argc - 1, &argv[1]);
	IOMultiplexing	iom(vec_port);

	std::cout << "サーバー起動!!" << std::endl;
	
	iom.IOMultiplexingLoop();
}

// https://github.com/kyamagis/http_socket.git