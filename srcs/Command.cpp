#include "../include/Command.hpp"

Command::Command(): _name(""), _args(0) {}
Command::Command(std::string line) {
	size_t start = 0;
	size_t end = line.find(' ');
	if (end == std::string::npos)
		throw(std::runtime_error("Invalid command"));
	_name = line.substr(start, end - start);
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
Command::Command(const Command &other) {
	if (this != &other) {
		_name = other.getName();
		_args = other.getArgs();
	}
}
Command &Command::operator=(const Command &other) {
	if (this != &other) {
		_name = other.getName();
		_args = other.getArgs();
	}
	return (*this);
}
Command::~Command() {}

const std::string &Command::getName(void) const {
	return (_name);
}

const std::vector<std::string> &Command::getArgs(void) const {
	return (_args);
}
