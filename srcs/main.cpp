
#include "../includes/IOMultiplexing.hpp"
#include "../includes/Config.hpp"

// vec_int_	makeVecPortsFormCharP(int argc, char **argv)
// {
// 	vec_int_	vec_ports;

// 	for(int i = 0; i < argc; i++)
// 	{
// 		vec_ports.push_back(atoi(argv[i]));
// 	}
// 	return vec_ports;
// }

int	main(int argc, char **argv)
{
	if (argc != 2)
		utils::exitWithError("invalid number of arguments\nusage: ./webserv config/default.conf");

	debug(argv[1]);
	Config config(argv[1]);

	config.readConfigFile();
	config.parseConfig();

	signal(SIGPIPE, SIG_IGN);

	IOMultiplexing	iom(config.getServers());

	debug("サーバー起動!!");

	iom.IOMultiplexingLoop();
}

// https://github.com/kyamagis/http_socket.git

// git config --local alias.rmp '!git checkout master && git pull https://github.com/kyamagis/http_socket.git master && git checkout - && git merge master'
