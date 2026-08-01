#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "./macros.hpp"

enum commandType {
	PASS,
	NICK,
	USER,
	PING,
	QUIT,
	JOIN,
	PART,
	PRIVMSG,
	NOTICE,
	MODE,
	TOPIC,
	KICK,
	INVITE,
	NAMES,
	LIST,
	WHO,
	WHOIS,
	HELP,
	NONE
};

class Command {
	private:
		std::string _name;
		std::vector<std::string> _args;
		commandType _type;
		void _parseLine(std::string line);
		void _setCommandType(void);
	public:
		Command();
		Command(std::string line);
		Command(const Command &other);
		Command &operator=(const Command &other);
		~Command();
		const std::string &getName(void) const;
		const commandType &getType(void) const;
		const std::vector<std::string> &getArgs(void) const;
};

#endif /* COMMAND_HPP */
