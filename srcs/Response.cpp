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
	client->appendToResponse(_buffer);
}

void	Response::_nickNameCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (client->getAuthenticated() == false)
		throw(std::runtime_error("Client not authenticated yet!"));
	if (args.size() != 1)
		throw(std::runtime_error("Invalid number of args for NICK command!"));
	client->setNickName(args[0]);
	_buffer += GREEN;
	_buffer += "Done, New nickname is [" + client->getNickName() + "]";
	_buffer += RESET;
	_buffer += "\r\n";
	if (client->getUserName().size() && client->getRegistered() == false)
	{
		client->setRegistered(true);
		_buffer += YEL;
		_buffer += "User: [" + client->getUserName() + "] registered successfully";
		_buffer += RESET;
		_buffer += "\r\n";
	}
	client->appendToResponse(_buffer);
}

void	Response::_quitCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 0)
		throw(std::runtime_error("Invalid number of args for QUIT command!"));
	_buffer += "By!";
	_buffer += "\r\n";
	client->setDisconnected(true);
	client->appendToResponse(_buffer);
}

void	Response::_userCmd(void) {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (client->getAuthenticated() == false)
		throw(std::runtime_error("Client not authenticated yet!"));
	if (args.size() < 4)
		throw(std::runtime_error("Invalid number of args for USER command!"));
	if (args[3][0] != ':')
		throw(std::runtime_error("Invalid realname-> [:realname]"));
	client->setUserName(args[0]);
	std::string realName = args[3];
	realName.erase(0, 1);
	if (realName.size() == 0 && args.size() == 4)
		throw(std::runtime_error("Invalid realname-> [:realname]"));
	if (realName.size() > 0)
		realName += " ";
	for (size_t i = 4; i < args.size(); i++)
		realName += args[i] + " ";
	realName.erase(realName.size() - 1);
	client->setRealName(realName);	
	_buffer += GREEN;
	_buffer += "Done, New username is [" + client->getUserName() + "]";
	_buffer += " and realname is [" + client->getRealName() + "]";
	_buffer += RESET;
	_buffer += "\r\n";
	if (client->getNickName().size() && client->getRegistered() == false)
	{
		client->setRegistered(true);
		_buffer += YEL;
		_buffer += "User: [" + client->getUserName() + "] registered successfully";
		_buffer += RESET;
		_buffer += "\r\n";
	}
	client->appendToResponse(_buffer);
}

void	Response::_passCmd()
{
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 1)
		throw(std::runtime_error("Invalid number of args for password command!"));
	if (client->getAuthenticated() == true) {
		_buffer.clear();
		_buffer += YEL;
		_buffer +=  "User already authenticated";
		_buffer += RESET;
		_buffer += "\r\n";
		return ;
	}
	if (args[0] != _password) {
		client->setDisconnected(true);
		throw(std::runtime_error("Invalid password!"));
	}
	client->setAuthenticated(true);
	_buffer += GREEN;
	_buffer += "Done, New client authenticated";
	_buffer += RESET;
	_buffer += "\r\n";
	client->appendToResponse(_buffer);
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
	channelName.erase(0, 1);
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
		if (client->isInvitedToChannel(channel))
		{
			client->appendChannels(channel);
			channel->addClient(client);
		}
		else
		{
			_buffer += RED ;
			_buffer += "this Channel is invite only\r\n";
			_buffer += RESET;
			return ;
		}
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
		client->appendChannels(channel);
		channel->addClient(client);
	}	 
	std::set<Client*> &members = channel->getMembers();
	_buffer += GREEN;
	_buffer += ":" + client->getNickName() + "!" + client->getUserName() + "@host JOIN #" + channelName;
	_buffer += RESET;
	_buffer += "\r\n";
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{	
		(*it)->appendToResponse(_buffer);
		server->sendResponse(*it);
	}
	_buffer.clear();
	if (channel->getTopic().size())
	{
		std::string topic;
		topic = GREEN;
		topic += ":server 332 " + client->getNickName() + " #" + channelName + " :" + channel->getTopic();
		topic += RESET;
		topic += "\r\n";
		client->appendToResponse(topic);
		server->sendResponse(client);
		topic.clear();
	}
	_buffer += GREEN;
	_buffer += ":server 353 " + client->getNickName() + " = #" + channelName + " :";
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{
		if (channel->getOperators().find(*it) != channel->getOperators().end())
			_buffer += "@";
		_buffer += (*it)->getNickName() + " ";
	}
	_buffer += "\r\n";
	_buffer += ":server 366 " + client->getNickName() + " #" + channelName + " :End of /NAMES list.";
	_buffer += RESET;
	_buffer += "\r\n";
	client->appendToResponse(_buffer);
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
	channelName.erase(0, 1);
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
	std::string nickname = kickedClient->getNickName();
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
	client->appendToResponse(_buffer);
}
void ::Response::_clearCmd() {
	std::vector<std::string> args = cmd.getArgs();
	if (args.size() != 0)
		throw(std::runtime_error("Invalid number of args for clear command!"));
	_buffer.clear();
	_buffer = "\x1B[3J\x1B[2J\x1B[H";
	client->appendToResponse(_buffer);
}

void ::Response::_unknownCmd() {
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	_buffer = SERVER_NAME;
	_buffer += " 421 " + client->getNickName() + " " + cmd.getName();
	_buffer += " :Unknown command\r\n";
	client->appendToResponse(_buffer);
	server->sendResponse(client);
}

void 	Response::_inviteCmd() {
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 2)
		throw(std::runtime_error("Invalid number of args for invite command!"));
	std::string channelName = args[1];
	if (channelName.size() < 2 || channelName[0] != '#')
		throw(std::runtime_error("Invalid channel name [# at the start]!"));
	channelName.erase(0, 1);
	Channel *channel = manager.find(channelName);
	if (channel == NULL)
		throw(std::runtime_error("There is no channel with this name!"));
	if (!channel->isOperator(client))
		throw(std::runtime_error("Your not operator!"));
	if (channel->isMemberByName(channelName) != NULL)
		throw(std::runtime_error("already a member!"));
	Client *invitedClient = server->getClientByName(args[0]);
	if (invitedClient == NULL)
		throw(std::runtime_error("There is no client with this name!"));
	invitedClient->addInvitedChannel(channel);
	_buffer += GREEN;
	_buffer += "Your invited to the channel: " + channelName;
	_buffer += RESET;
	_buffer += "\r\n";
	invitedClient->appendToResponse(_buffer);
	server->sendResponse(invitedClient);
}

void 	Response::_topicCmd() {
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 	1 && args.size() < 2)
		throw(std::runtime_error("Invalid number of args for topic command!"));
	std::string channelName = args[0];
	if (channelName.size() < 2 || channelName[0] != '#')
		throw(std::runtime_error("Invalid channel name [# at the start]!"));
	channelName.erase(0, 1);
	Channel *channel = manager.find(channelName);
	if (channel == NULL)
		throw(std::runtime_error("There is no channel with this name!"));
	std::string topic;
	if (args.size() == 1)
	{
		topic = channel->getTopic();
		if (topic.size() == 0)
		{
			topic += ":server 331 " + client->getNickName() + " #" + channelName + " :No topic is set";
			throw(std::runtime_error(topic));
		}
		_buffer += GREEN;
		_buffer += ":server 332 " + client->getNickName() + " #" + channelName + " :" + topic;
		_buffer += RESET;
		_buffer += "\r\n";
		client->appendToResponse(_buffer);
		server->sendResponse(client);
		_buffer.clear();
		return ;
	}
	if (args[1].size() < 2 || args[1][0] != ':')
		throw(std::runtime_error("Invalid start of topic, need to start with `:`!"));
	if (!channel->isMember(client))
		throw(std::runtime_error("Not a member!"));
	if (channel->isTopicRestricted() && !channel->isOperator(client))
		throw(std::runtime_error("Not an operator!"));
	_buffer += GREEN;
	_buffer += ":" + client->getNickName() + "!" + client->getUserName() + "@" + SERVER_NAME + "TOPIC #" + channelName + " :";
	args[1].erase(0, 1);
	for (size_t i = 1; i < args.size(); i++){
		topic += args[i] + " ";
		_buffer += args[i] + " ";
	}
	_buffer.erase(_buffer.size() - 1);
	topic.erase(topic.size() - 1);
	_buffer += RESET;
	_buffer += "\r\n";
 	std::set<Client*> &members = channel->getMembers();
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{
		(*it)->appendToResponse(_buffer);
		server->sendResponse(*it);
	}
	channel->setTopic(topic);
	_buffer.clear();
}

void	Response::_privMsgCmd() {
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() < 2)
		throw(std::runtime_error("At least two arguments are needed: <Target> and <Message>"));
	if (args[0][0] == '#') {
		    args[0].erase(std::remove(args[0].begin(), args[0].end(), '#'), args[0].end());
		Channel *target = manager.find(args[0]);
		if (target == NULL)
			throw(std::runtime_error("Target channel doesn't exits"));
		std::set<Client *> clients = target->getMembers();
		if (std::find(clients.begin(), clients.end(), client) == clients.end())
			throw(std::runtime_error("you are not a member in the target channel"));
		for (std::set<Client *>::iterator it = clients.begin(); it != clients.end(); it++) {
			if (*it != client) {
				_buffer = ":" + client->getNickName() + "!";
				_buffer += client->getUserName();
				_buffer += "@";
				_buffer += SERVER_NAME;
				_buffer += " PRIVMSG ";
				_buffer += target->getName();
				_buffer += " ";
				for (size_t i  = 1; i < args.size(); i += 1)
					_buffer += args[i] + " ";
				_buffer += "\r\n";
				(*it)->appendToResponse(_buffer);
				server->sendResponse(*it);
			}
		}

	}
	else {
		LOG("Looking for client " << args[0])
		Client *target = server->getClientByName(args[0]);
		if (target == NULL)
			throw(std::runtime_error("Target client doesn't exits"));
		else
		LOG("Client found!");
		_buffer.clear();
		_buffer = ":" + client->getNickName() + "!";
		_buffer += client->getUserName();
		_buffer += "@";
		_buffer += SERVER_NAME;
		_buffer += "PRIVMSG ";
		_buffer += target->getNickName();
		_buffer += " ";
		for (size_t i  = 1; i < args.size(); i += 1)
			_buffer += args[i] + " ";
		_buffer += "\r\n";
		target->appendToResponse(_buffer);
		server->sendResponse(target);
	}
}

void	Response::_listCmd() {
	if (cmd.getArgs().size() != 0)
		throw(std::runtime_error("List command is not supposed to have any arguments!"));
	std::set<Channel *> channels = client->getChannels();
	for (std::set<Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {
		_buffer.clear();
		_buffer  += ":";
		_buffer += SERVER_NAME;
		_buffer += " 322 ";
		_buffer += client->getNickName();
		_buffer += " " + (*it)->getName() + " ";
		std::stringstream stream;
		stream << (*it)->getNMembers();
		_buffer +=  stream.str();
		_buffer += ": " + (*it)->getTopic();
		_buffer += "\r\n";
		client->appendToResponse(_buffer);
		server->sendResponse(client);
	}
}

void	Response::_whoCmd() {
	std::vector<std::string> args = cmd.getArgs();
	if (args.size() > 2)
		throw("Too many arguments, max is 2!");
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client is not registered !"));
	_buffer.clear();
	if (args.size() == 0) {
		std::map<int, Client *> clients = server->getClients();
		for (std::map<int, Client *>::iterator it = clients.begin(); it != clients.end(); it++)
		{
			_buffer += ":IRC98 352 " + client->getNickName() + " ";
			_buffer += (it->second->getRegistered() ? it->second->getUserName() : "*") + " ";
			_buffer +=  SERVER_NAME;
			_buffer += " IRC98 " + it->second->getNickName() + " H@ :0 ";
			_buffer += it->second->getRealName();
			_buffer += "\r\n";
		}
			client->appendToResponse(_buffer);
			server->sendResponse(client);
	}
	else if (args.size() == 1 && args[0][0] == '#') {
		args[0].erase(std::remove(args[0].begin(), args[0].end(), '#'), args[0].end());
		Channel *target = manager.getOrCreateChan(args[0]);
		if (target == NULL)
			throw(std::runtime_error("Target channel doesn't exists !"));
		std::set<Client *> clients;
		if (args.size() == 2 && args[0][1] != 'o')
					throw(std::runtime_error("Invalid option !"));
		else if (args.size() == 2 && args[0][1] == 'o')
			clients = target->getOperators();
		else
			clients = target->getMembers();
		for (std::set<Client *>::iterator it = clients.begin(); it != clients.end(); it++) {
			_buffer += ":IRC98 352 " + client->getNickName() + " ";
			_buffer += ((*it)->getRegistered() ? (*it)->getUserName() : "*") + " ";
			_buffer += " ";
			_buffer += SERVER_NAME;
			_buffer += " IRC98 " + (*it)->getNickName() + " H@ :0 ";
			_buffer += (*it)->getRealName();
		}
			client->appendToResponse(_buffer);
			server->sendResponse(client);
	}
	else {
			Client *target = server->getClientByName(args[0]);
			if (target == NULL)
				throw(std::runtime_error("target client not found"));
			_buffer += ":IRC98 352 " + client->getNickName() + " ";
			_buffer += (target->getRegistered() ? target->getUserName() : "*") + " ";
			_buffer += " ";
			_buffer += SERVER_NAME;
			_buffer += " IRC98 " + target->getNickName() + " H@ :0 ";
			_buffer += target->getRealName();
			client->appendToResponse(_buffer);
			server->sendResponse(client);
	}

}

void	Response::runCmd() {

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
	else if (cmd.getType() == INVITE)
		_inviteCmd();
	else if (cmd.getType() == TOPIC)
		_topicCmd();
	else if (cmd.getType() == PRIVMSG)
		_privMsgCmd();
	else if (cmd.getType() == LIST)
		_listCmd();
	else if (cmd.getType() == WHO)
		_whoCmd();
	else if (cmd.getType() == UNKNOWN)
		_unknownCmd();
	else {
		std::cout << "Type is : " << cmd.getType() << std::endl;
		throw(std::runtime_error("(" + cmd.getName() + ")" + ": Not implemented yet!"));
	}
}
