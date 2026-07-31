#include "../include/Response.hpp"

Response::Response(): _buffer(""){}
Response::Response(Command &c, Client &cl): _buffer(""), cmd(c), client(cl) {}
Response::Response(const Response &other): _buffer(other._buffer),
	cmd(other.cmd), client(other.client) {}
Response &Response::operator=(const Response &other) {
	if (this != &other) {
		cmd = other.cmd;
		client = other.client;
		_buffer = other._buffer;
	}
	return (*this);
}
Response::~Response() {}


std::string Response::getBuffer(void) const { return (_buffer); }

void	Response::_helpCmd() {
	_buffer.clear();
	_buffer += YEL;
	_buffer += "Man Page of ircserv:\nUsage: <COMMAND> <ARGS>\n\nAvailable Commands:\n\n";
	_buffer += "HELP: Shows this help menu\nNICK: Sets the user nickname on the server and Displays it\n";
	_buffer += RESET;
	_buffer += "\n\r";
}

void	Response::_nickNameCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() < 1)
		throw("Invalid number of args for NICK command!");
	client.setNickname(args[0]);
	_buffer += client.getNickname();
	_buffer += "\n\r";
}

void	Response::runCmd() {

	if (cmd.getType() == HELP)
		_helpCmd();
	else
		throw(std::runtime_error("(" + cmd.getName() + ")" + ": Not implemented yet!"));
}
