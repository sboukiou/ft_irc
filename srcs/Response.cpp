#include "../include/Response.hpp"

Response::Response(Command &c, Client *cl, std::string pass, ChannelManager &manager, Server *server): _buffer(""), cmd(c), client(cl), _password(pass), manager(manager), server(server) {}
Response::Response(const Response &other): _buffer(other._buffer),
	cmd(other.cmd), client(other.client), manager(other.manager) {}
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
	_buffer += "             [man  3 ircserv]                 \n\n* Usage: <COMMAND> <ARGS>\nAvailable Commands:\n";
	_buffer += "* [HELP] [NICK] [UESR] [QUIT] [CLEAR]\n";
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

void	Response::_userCmd(void) {
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
		throw(std::runtime_error("Invalid number of args for password command!"));
	if (args[0] != _password)
		throw(std::runtime_error("Invalid password!"));
	client->setAuthenticated(true);
	_buffer += GREEN;
	_buffer += "Done, New client authenticated";
	_buffer += RESET;
	_buffer += "\r\n";
}

void	Response::_joinCmd()
{
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if ((args.size() != 2 && args.size() != 1))
		throw(std::runtime_error("Invalid number of args for USER command!"));
	std::string channelName = args[0];
	if (channelName.size() < 2 || channelName[0] != '#')
		throw(std::runtime_error("Invalid channel name [# at the start]!"));
	channelName = channelName.substr(1, channelName.size() - 1);
	Channel *channel = manager.find(channelName);
	if (channel == NULL)
	{
		if (args.size() == 1)
			channel = manager.getOrCreateChan(channelName);
		else
			channel = manager.getOrCreateChan(channelName, args[1]);
		client->appendChannels(channel);
		channel->addClient(client);
		channel->addOperator(client);
	}
	else if (channel->isMember(client))
	{
		_buffer += YEL ;
		_buffer += "Client already a member\r\n";
		_buffer += RESET;
		return ;
	}
	else if (channel->getInviteOnly())
	{
		_buffer += RED ;
		_buffer += "this Channel is invite only\r\n";
		_buffer += RESET;
		return ;
	}
	else if (channel->getUserLimit() && channel->getNMembers() >= channel->getMemberCount())
	{
		_buffer += RED ;
		_buffer += "this Channel is full\r\n";
		_buffer += RESET;
		return ;
	}
	else if (channel->getChannelPass() && args.size() == 2 && args[1] != channel->getPass())
	{
		_buffer += RED ;
		_buffer += "Password is incorrect\r\n";
		_buffer += RESET;
		return ;
	}
	else if (channel->getChannelPass() && args.size() == 1)
	{
		_buffer += RED ;
		_buffer += "this channel required a password\r\n";
		_buffer += RESET;
		return ;
	}
	else
	{
		channel = manager.getOrCreateChan(channelName);
		client->appendChannels(channel);
		channel->addClient(client);
	}
	std::set<Client*> &members = channel->getMembers();
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{	
		_buffer += GREEN;
		_buffer += ":" + client->getNickname() + "!" + client->getUsername() + "@host Join #" + channelName;
		_buffer += RESET;
		_buffer += "\r\n";
		(*it)->appendToResponse(_buffer);
		server->sendResponse(*it);
		_buffer.clear();
	}
	_buffer += GREEN;
	_buffer += ":server 353 " + client->getNickname() + " = #" + channelName + ":";
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{
		if (channel->getOperators().find(*it) != channel->getOperators().end())
			_buffer += "@";
		_buffer += (*it)->getNickname() + " ";
	}
	_buffer += "\r\n";
	_buffer += ":server 366 " + client->getNickname() + " #" + channelName + " :End of /NAMES list.";
	_buffer += RESET;
	_buffer += "\r\n";
}

void	Response::_kickCmd()
{
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if ((args.size() != 2 && args.size() != 3))
		throw(std::runtime_error("Invalid number of args for USER command!"));
	std::string channelName = args[0];
	if (channelName.size() < 2 || channelName[0] != '#')
		throw(std::runtime_error("Invalid channel name [# at the start]!"));
	channelName = channelName.substr(1, channelName.size() - 1);
	Channel *channel = manager.find(channelName);
	if (channel == NULL)
		throw(std::runtime_error("There is no channel with this name!"));
	if (!channel->isOperator(client))
		throw(std::runtime_error("Your not operator!"));
	Client* kickedClient = channel->isMemberByName(args[1]);
	if (kickedClient == NULL)
		throw(std::runtime_error("No client with this nickName!"));
	_buffer += YEL;
	_buffer += "Your kicked from the channel: " + channelName;
	if (args.size() == 3)
		_buffer += " because of " + args[2];
	_buffer += RESET;
	_buffer += "\r\n";
	std::string nickname = kickedClient->getNickname();
	kickedClient->appendToResponse(_buffer);
	server->sendResponse(kickedClient);
	channel->removeClient(kickedClient);
	if (client->getDisconnected() == true)
		server->removeClient(kickedClient);
	_buffer.clear();
	_buffer += YEL;
	_buffer += nickname + " got kicked from the channel: " + channelName;
	if (args.size() == 3)
		_buffer += " because of " + args[2];
	_buffer += RESET;
	_buffer += "\r\n";
	std::set<Client*> &members = channel->getMembers();
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{
		(*it)->appendToResponse(_buffer);
		server->sendResponse(*it);
	}
	_buffer.clear();
}
void	Response::_pingCmd()
{
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 1)
		throw(std::runtime_error("Invalid number of args for USER command!"));
	_buffer += GREEN;
	_buffer += "PONG ";
	_buffer += args[0];
	_buffer += RESET;
	_buffer += "\r\n";
}
void ::Response::_clearCmd() {
	std::vector<std::string> args = cmd.getArgs();
	if (args.size() != 0)
		throw(std::runtime_error("Invalid number of args for clear command!"));
	_buffer.clear();
	_buffer = "\x1B[3J\x1B[2J\x1B[H";
}

void ::Response::_broadCastCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	_buffer = cmd.getName();
	for (size_t i = 0; i < args.size(); i += 1)
		_buffer += args[i];
	/* TODO: Implement the Broadcasting logic  */
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
		else if (cmd.getType() == JOIN)
			_joinCmd();
		else if (cmd.getType() == KICK)
			_kickCmd();
		else if (cmd.getType() == PING)
			_pingCmd();
		else if (cmd.getType() == CLEAR)
			_clearCmd();
		else if (cmd.getType() == BROADCAST)
			_broadCastCmd();
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
