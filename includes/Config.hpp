#ifndef CONFIG_HPP
# define CONFIG_HPP

# include	<vector>
# include	<iostream>
# include	"Server.hpp"
# include	"utils.hpp"

# define CONF_EXPANSION_LEN 5

class Config
{
	private:
		str_				_file_name;
		vec_str_			_config_lines;
		vec_str_iter_		_line_itr;
		vec_str_iter_		_end_line_itr;
		std::vector<Server>	_servers;
		std::vector<int>	_ports;

		void	_isValidFileFormat(str_ file_name);
		void	_trimComments(str_& str);
		void	_trimSpaces(str_& str);
		str_	_getPushBackLine(str_& str);
		void	_checkValidSemicolon(size_t line_pos, str_& str);
		void	_getValidPorts(void);

	public:
		Config(str_ file_name);
		~Config();
		void				readConfigFile(void);
		void				parseConfig(void);
		std::vector<Server>	getServers(void) const;
		std::vector<int>	getPorts(void) const;
		void				printConfig(void);
};

#endif
