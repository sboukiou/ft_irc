#ifndef COMMAND_HPP
# define COMMAND_HPP

#include <string>
#include <vector>
enum commandType {
	PASS,
	NICK,
	USER,
	PING,
	PONG,
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
};

class Command {
	private:
		std::string _name;
		std::vector<std::string> _args;
		commandType _type;
	public:
		Command();
		Command(std::string line);
		Command(const Command &other);
		Command &operator=(const Command &other);
		~Command();
		const std::string &getName(void) const;
		const std::vector<std::string> &getArgs(void) const;
};

#endif /* COMMAND_HPP */
