#include "../include/Command.hpp"

Command::Command(): _name(""), _args(0) {
}
Command::Command(std::string line) {
	_parseLine(line);
	_setCommandType();
}
Command::Command(const Command &other) {
	if (this != &other) {
		_name = other.getName();
		_args = other.getArgs();
		_setCommandType();
	}
}
Command &Command::operator=(const Command &other) {
	if (this != &other) {
		_name = other.getName();
		_args = other.getArgs();
		_setCommandType();
	}
	return (*this);
}
Command::~Command() {}

const std::string &Command::getName(void) const {
	return (_name);
}
const commandType &Command::getType(void) const {
	return (_type);
}

const std::vector<std::string> &Command::getArgs(void) const {
	return (_args);
}

void Command::_parseLine(std::string line) {
	size_t start = 0;
	size_t end = line.find(' ');
	_name = line.substr(start, end - start);
	if (end == std::string::npos)
		return ;
	start = end + 1;
	while (line[start] == ' ')
		start += 1;
	while (start < line.size() && end != std::string::npos) {
		end = line.find(' ', start);
		_args.push_back(line.substr(start, end -  start));
		start = end + 1;
		while (line[start] == ' ')
			start += 1;
	}
}


void Command::_setCommandType(void) {
	if (_name == "PASS")
		_type = PASS;
	else if (_name == "NICK")
		_type = NICK;
	else if (_name == "USER")
		_type = USER;
	else if (_name == "PING")
		_type = PING;
	else if (_name == "QUIT")
		_type = QUIT;
	else if (_name == "JOIN")
		_type = JOIN;
	else if (_name == "PART")
		_type = PART;
	else if (_name == "PRIVMSG")
		_type = PRIVMSG;
	else if (_name == "NOTICE")
		_type = NOTICE;
	else if (_name == "MODE")
		_type = MODE;
	else if (_name == "TOPIC")
		_type = TOPIC;
	else if (_name == "KICK")
		_type = KICK;
	else if (_name == "INVITE")
		_type = INVITE;
	else if (_name == "NAMES")
		_type = NAMES;
	else if (_name == "LIST")
		_type = LIST;
	else if (_name == "WHO")
		_type = WHO;
	else if (_name == "WHOIS")
		_type = WHOIS;
	else if (_name == "HELP")
		_type = HELP;
	else {
		WARN("NO type was found");
		_type = NONE;
	}
}
