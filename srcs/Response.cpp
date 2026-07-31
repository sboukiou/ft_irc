#include "../include/Response.hpp"

Response::Response(): _buffer(""){}
Response::Response(Command &c, Client *cl, std::string pass): _buffer(""), cmd(c), client(cl), _password(pass) {}
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
	_buffer += "Man Page of ircserv:\nUsage: <COMMAND> <ARGS>\nAvailable Commands:\n";
	_buffer += "HELP: Shows this help menu\nNICK: Sets the user nickname on the server";
	_buffer += RESET;
	_buffer += "\n\r";
}

void	Response::_nickNameCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (client->getAuthenticated() == false)
		throw(std::runtime_error("Client not authenticated yet!"));
	if (args.size() != 1)
		throw(std::runtime_error("Invalid number of args for NICK command!"));
	client->setNickname(args[0]);
	_buffer += GREEN;
	_buffer += "Done, New nickname is [" + client->getNickname() + "]";
	_buffer += RESET;
	_buffer += "\r\n";
	if (client->getUsername().size() && client->getRegistered() == false)
	{
		client->setRegistered(true);
		_buffer += YEL;
		_buffer += "User: [" + client->getUsername() + "] registered successfully";
		_buffer += RESET;
		_buffer += "\r\n";
	}
}

void	Response::_quitCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 0)
		throw(std::runtime_error("Invalid number of args for QUIT command!"));
	_buffer += "By!";
	_buffer += "\r\n";
	client->setDisconnected(true);
}

void	Response::_userCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (client->getAuthenticated() == false)
		throw(std::runtime_error("Client not authenticated yet!"));
	if (args.size() != 4)
		throw(std::runtime_error("Invalid number of args for USER command!"));
	std::string realName = args[3];
	if (realName.size() < 2 || realName[0] != ':')
		throw(std::runtime_error("Invalid realname-> [:realname]"));
	client->setUsername(args[0]);
	
	client->setRealname(realName.substr(1, realName.size() - 1)); 
	_buffer += GREEN;
	_buffer += "Done, New username is [" + client->getUsername() + "]";
	_buffer += " and realname is [" + client->getRealkname() + "]";
	_buffer += RESET;
	_buffer += "\r\n";
	if (client->getNickname().size() && client->getRegistered() == false)
	{
		client->setRegistered(true);
		_buffer += YEL;
		_buffer += "User: [" + client->getUsername() + "] registered successfully";
		_buffer += RESET;
		_buffer += "\r\n";
	}
}

void	Response::_passCmd()
{
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 1)
		throw(std::runtime_error("Invalid number of args for USER command!"));
	if (args[0] != _password)
		throw(std::runtime_error("Invalid password!"));
	client->setAuthenticated(true);
	_buffer += GREEN;
	_buffer += "Done, New client authenticated";
	_buffer += RESET;
	_buffer += "\r\n";
}

void	Response::runCmd() {

	try {
		if (cmd.getType() == HELP)
			_helpCmd();
		else if (cmd.getType() == NICK)
			_nickNameCmd();
		else if (cmd.getType() == QUIT)
			_quitCmd();
		else if (cmd.getType() == USER)
			_userCmd();
		else if (cmd.getType() == PASS)
			_passCmd();
		else {
			std::cout << "Type is : " << cmd.getType() << std::endl;
			throw(std::runtime_error("(" + cmd.getName() + ")" + ": Not implemented yet!"));
		}
	}
	catch (std::exception &e) {
		_buffer.clear();
	_buffer += RED;
	_buffer += "Error: ";
	_buffer += e.what();
	_buffer += RESET;
	_buffer += "\r\n";
	}
}
