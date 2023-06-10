
#include "../includes/IOMultiplexing.hpp"
#include "../includes/Config.hpp"

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
