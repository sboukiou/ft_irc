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
	_buffer += "             [man  3 ircserv]                 \n\n* Usage: <COMMAND> <ARGS>\nAvailable Commands:\n";
	_buffer += "* [HELP] [NICK] [UESR] [QUIT] [CLEAR]\n";
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
	_buffer += "Done, New nickname is [" + client->getNickName() + "]";
	_buffer += "\r\n";
	if (client->getUserName().size() && client->getRegistered() == false)
	{
		client->setRegistered(true);
		_buffer += "User: [" + client->getUserName() + "] registered successfully";
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
	_buffer += "Done, New username is [" + client->getUserName() + "]";
	_buffer += " and realname is [" + client->getRealName() + "]";
	_buffer += "\r\n";
	if (client->getNickName().size() && client->getRegistered() == false)
	{
		client->setRegistered(true);
		_buffer += "User: [" + client->getUserName() + "] registered successfully";
		_buffer += "\r\n";
	}
	client->appendToResponse(_buffer);
}

void	Response::_passCmd()
{
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() != 1)
		throw(std::runtime_error("Invalid number of args for password command!\r\n"));
	if (client->getAuthenticated() == true) {
		_buffer.clear();
		_buffer +=  "User already authenticated";
		_buffer += "\r\n";
		return ;
	}
	if (args[0] != _password) {
		client->setDisconnected(true);
		throw(std::runtime_error("Invalid password!"));
	}
	client->setAuthenticated(true);
	_buffer += "Done, New client authenticated";
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
		throw(std::runtime_error("Invalid number of args for JOIN command!"));
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
		channel->addClient(client);
		channel->addOperator(client);
	}
	else if (channel->isMember(client))
	{
		_buffer += "Client already a member\r\n";
		return ;
	}
	else if (channel->getUserLimit() && channel->getMemberCount() >= channel->getMaxMembers())
	{
		_buffer += "this Channel is full\r\n";
		return ;
	}
	else if (channel->getChannelPass() && args.size() == 2 && args[1] != channel->getPass())
	{
		_buffer += "Password is incorrect\r\n";
		return ;
	}
	else if (channel->getChannelPass() && args.size() == 1)
	{
		_buffer += "this channel required a password\r\n";
		return ;
	}
	else if (channel->getInviteOnly())
	{
		if (client->isInvitedToChannel(channel))
			client->removeInvitedChannel(channel);
		else
		{
			_buffer += "this Channel is invite only\r\n";
			return ;
		}
	}
	if (channel->isMember(client) == false)
		channel->addClient(client);
	client->appendChannels(channel);
	std::set<Client*> &members = channel->getMembers();
	_buffer += ":" + client->getNickName() + "!" + client->getUserName() + "@host JOIN #" + channelName;
	_buffer += "\r\n";
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{	
		(*it)->appendToResponse(_buffer);
		server->sendResponse(*it);
	}
	_buffer.clear();
	if (channel->getTopic().empty() == false)
	{
		std::string topic;
		topic += ":server 332 " + client->getNickName() + " #" + channelName + " :" + channel->getTopic();
		topic += "\r\n";
		client->appendToResponse(topic);
		server->sendResponse(client);
		topic.clear();
	}
	std::set<Client*>::iterator last = members.end();
	--last;
	_buffer += ":server 353 " + client->getNickName() + " = #" + channelName + " :";
	for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
	{
		if (channel->getOperators().find(*it) != channel->getOperators().end())
			_buffer += "@";
		_buffer += (*it)->getNickName();
		if (it != last)
			_buffer += " ";
	}
	_buffer += "\r\n";
	_buffer += ":server 366 " + client->getNickName() + " #" + channelName + " :End of /NAMES list.";
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
		throw(std::runtime_error("Invalid number of args for KICK command!"));
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
	_buffer += "Your kicked from the channel: " + channelName;
	if (args.size() == 3)
		_buffer += " because of " + args[2];
	_buffer += "\r\n";
	std::string nickname = kickedClient->getNickName();
	kickedClient->appendToResponse(_buffer);
	server->sendResponse(kickedClient);
	channel->removeClient(kickedClient);
	if (client->getDisconnected() == true)
		server->removeClient(kickedClient);
	_buffer.clear();
	_buffer += nickname + " got kicked from the channel: " + channelName;
	if (args.size() == 3)
		_buffer += " because of " + args[2];
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
	_buffer += "PONG ";
	_buffer += args[0];
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
	_buffer += "Your invited to the channel: " + channelName;
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
		_buffer += ":server 332 " + client->getNickName() + " #" + channelName + " :" + topic;
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
	_buffer += ":" + client->getNickName() + "!" + client->getUserName() + "@" + SERVER_NAME + "TOPIC #" + channelName + " :";
	args[1].erase(0, 1);
	for (size_t i = 1; i < args.size(); i++){
		topic += args[i] + " ";
		_buffer += args[i] + " ";
	}
	_buffer.erase(_buffer.size() - 1);
	topic.erase(topic.size() - 1);
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
		Client *target = server->getClientByName(args[0]);
		if (target == NULL)
			throw(std::runtime_error("Target client doesn't exits"));
		_buffer.clear();
		_buffer = ":" + client->getNickName() + "!";
		_buffer += client->getUserName();
		_buffer += "@";
		_buffer += SERVER_NAME;
		_buffer += " PRIVMSG ";
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
		stream << (*it)->getMaxMembers();
		_buffer +=  stream.str();
		_buffer += ": " + (*it)->getTopic();
		_buffer += "\r\n";
		client->appendToResponse(_buffer);
		server->sendResponse(client);
	}
}

void Response::_modeCmd()
{
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() < 2)
		throw(std::runtime_error("Invalid number of args for topic command!"));
	std::string channelName = args[0];
	if (channelName.size() < 2 || channelName[0] != '#')
		throw(std::runtime_error("Invalid channel name [# at the start]!"));
	channelName.erase(0, 1);
	Channel *channel = manager.find(channelName);
	if (channel == NULL)
		throw(std::runtime_error("There is no channel with this name!"));
	std::string mode = args[1];
	if (mode.size() < 2 || (mode[0] != '+' && mode[0] != '-'))
		throw(std::runtime_error("Invalid start of mode '- | +'!"));
	if (channel->isMember(client) == false)
		throw(std::runtime_error("Not a member!"));
	if (channel->isOperator(client) == false)
		throw(std::runtime_error("Not an operator!"));
	char op = mode[0];
	size_t argIndex = 2;
	int notE = 0;
	for (size_t i = 1; i < mode.size(); i++)
	{
		if (mode[i] == 'i')
			channel->setInviteOnly((op == '+') ? true : false);
		else if (mode[i] == 't')
			channel->setTopicRestricted((op == '+') ? true : false);
		else if (mode[i] == 'l'){
			if (op == '+'){
				if (argIndex >= args.size()){
					notE = 1;
					break;
				}
				char *endptr;
				errno = 0;
				double value = strtod(args[argIndex].c_str(), &endptr);
				if (errno == ERANGE || *endptr || value < 1 || value > 2147483647){
					_buffer.clear();
					_buffer += ":server NOTICE <" + client->getNickName() + "> Invalid channel limit";
					_buffer += "\r\n";
				}
				else{
					channel->setUserLimit(true);
					channel->setMaxMembers((int)value);
				}
				argIndex++;
			}
			else
				channel->setUserLimit(false);
		}
		else if (mode[i] == 'k'){
			if (argIndex >= args.size()){
					notE = 1;
					break;
				}
			if (op == '+'){
				channel->setPass(args[argIndex]);
				channel->setChannelPass(true);
			}
			else{
				if (channel->getPass() != args[argIndex]){
					_buffer.clear();
					_buffer += ":server NOTICE <" + client->getNickName() + "> incorrect password";
					_buffer += "\r\n";
				}
				else{
					channel->setPass("");
					channel->setChannelPass(false);				
				}
			}
			argIndex++;
		}
		else if (mode[i] == 'o'){
			if (argIndex >= args.size()){
				notE = 1;
				break;
			}
			Client* addedOp = server->getClientByName(args[argIndex]);
			if (addedOp == NULL){
				_buffer.clear();
				_buffer += ":server NOTICE <" + client->getNickName() + "> there is no client with this name: " + args[argIndex];
				_buffer += "\r\n";
			}
			else if (channel->isMemberByName(args[argIndex]) == NULL){
				_buffer.clear();
				_buffer += ":server NOTICE <" + client->getNickName() + "> this client: " + args[argIndex] + " is not a member";
				_buffer += "\r\n";
			}
			else if (op == '+'){
				if (channel->isOperator(addedOp) == false)
					channel->addOperator(addedOp);
			}
			else{
				if (channel->isOperator(addedOp))
					channel->removeOperator(addedOp);
			}
			argIndex++;
		}
		else{
			_buffer.clear();
			_buffer += ":server 472 " + client->getNickName() + " ";
			_buffer += mode[i];
			_buffer += " :is unknown mode char to me\r\n";
		}
		if (_buffer.size() > 0){
			server->sendResponse(client);
			_buffer.clear();
		}
	}
	_buffer.clear();
	if (notE){
		_buffer += ":server 461 <" + client->getNickName() + "> MODE :Not enough parameters";
		_buffer += "\r\n";
		server->sendResponse(client);
		_buffer.clear();
	}
}

void	Response::_whoCmd() {
	std::vector<std::string> args = cmd.getArgs();
	if (args.size() > 2)
		throw("Too many arguments, max is 2!");
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

void	Response::_namesCmd() {
	std::vector<std::string> args = cmd.getArgs();

	_buffer.clear();
	_buffer += ":";
	_buffer += SERVER_NAME;
	_buffer += " 353 " + (client->getRegistered() ? client->getNickName(): "*");
	_buffer += " = ";
	if (args.size() == 0) {
		std::map<std::string, Channel *> channels = manager.getChannels();
		for (std::map<std::string, Channel *>::iterator it = channels.begin(); it != channels.end(); it++) {
			std::set<Client *> members = it->second->getMembers();
			_buffer += "#" + it->second->getName() + " : ";
			for (std::set<Client *>::iterator mem = members.begin(); mem != members.end(); mem++) {
				if (it->second->isOperator(*mem) == true)
					_buffer += "@";
				_buffer += (*mem)->getNickName() + " ";
			}
			_buffer += "\n";
		}
	}
	else {
		for (size_t i  = 0; i < args.size(); i += 1) {
		    args[i].erase(std::remove(args[i].begin(), args[i].end(), '#'), args[i].end());
			Channel *chan = manager.getOrCreateChan(args[i]);
			_buffer += chan->getName() + ": ";
			std::set<Client *> members = chan->getMembers();
			for (std::set<Client *>::iterator mem = members.begin(); mem != members.end(); mem++) {
				if (chan->isOperator(*mem) == true)
					_buffer += "@";
				_buffer += (*mem)->getNickName() + " ";
			}
			_buffer += "\n";
		}
	}
	_buffer += "\r\n";
	client->appendToResponse(_buffer);
	server->sendResponse(client);
}


void	Response::_broadCastToChannel(Channel *chan, std::string message, bool includeSender) {
	if (chan == NULL)
		throw(std::runtime_error("Channel not found!"));
	std::set<Client *> members = chan->getMembers();
	for (std::set<Client *>::iterator it =  members.begin(); it != members.end(); it++) {
		if ((*it) != client || includeSender) {
			(*it)->appendToResponse(message);
			server->sendResponse(*it);
		}
	}
}

void	Response::_partCmd() {
	std::vector<std::string> args = cmd.getArgs();

	if (client->getRegistered() == false)
		throw(std::runtime_error(":penguin 451 * :You have not registered "));
	if (args.size() == 0)
		throw(std::runtime_error(":penguin 461 PART :Not enough parameters"));
	size_t start = 0;
	for (start = 0; start < args.size(); start += 1)
		if (args[start][0] == ':')
			break ;
	if (start == 0)
		throw(std::runtime_error(":penguin 461 :Invalid token"));
	std::string reason;
	for (size_t i = start; i < args.size(); i += 1)
		reason += args[i] + " ";
	for (size_t i = 0; i < start; i += 1) {
		if (args[i].empty() || args[i][0] != '#') {
			client->appendToResponse(":penguin 403 " + client->getNickName() + 
					" " + args[i] + " :No such channel\r\n");
			server->sendResponse(client);
			continue ;
		}
		std::string name = args[i].substr(1);
		if (name.empty()) {
			client->appendToResponse(":penguin 403 " + client->getNickName()
					+ " " + args[i] + " :No such channel\r\n");
			server->sendResponse(client);
			continue ;
		}

		Channel *chan = manager.find(name);
		if (chan == NULL) {
			client->appendToResponse(":penguin 403 " + client->getNickName()
					+ " #" + name + " :No such channel\r\n");
			server->sendResponse(client);
			continue ;
		}
		if (chan->isMember(client) == false) {
			client->appendToResponse(":penguin 442 " + client->getNickName() + " #" + name + " :You're not on that channel\r\n");
			server->sendResponse(client);
			continue ;
		}
		std::string message = ":" + client->getNickName() + "!" + client->getUserName() + "@";
		message += SERVER_NAME;
		message += " PART #" + chan->getName() + " ";
		message += reason;
		message += "\r\n";
		_broadCastToChannel(chan, message, true);
		chan->removeClient(client);
		client->removeChannel(chan);
		manager.removeIfEmpty(std::string(chan->getName()));
	}
}

void	Response::_whoisCmd() {
	std::vector<std::string> args = cmd.getArgs();
	if (args.size() == 0)
		throw(std::runtime_error(":penguin 431 ali :No nickname given"));
	if (client->getRegistered() == false)
		throw(std::runtime_error(":penguin 451 * :You have not registered"));
	_buffer.clear();
	for (size_t i = 0; i < args.size(); i += 1) {
		Client *target = server->getClientByName(args[i]);
		if (target == NULL) {
			_buffer = ":";
			_buffer += SERVER_NAME;
			_buffer += "401 " + client->getUserName() + " " + args[i];
			_buffer += " :No such nick/channel";
		}
		else {
		_buffer += ":";
		_buffer += SERVER_NAME;
		_buffer += " 311 " + client->getUserName() + " " + target->getUserName();
		_buffer += " <target_address>  * " + target->getRealName();
		_buffer += "\n";
		_buffer += ":";
		_buffer += SERVER_NAME;
		_buffer += " 319 " + client->getUserName() + " " + target->getUserName();
		std::set<Channel *> chans = target->getChannels();
		_buffer += " :";
		for (std::set<Channel *>::iterator it = chans.begin(); it != chans.end(); it++) {
			_buffer += (*it)->isOperator(target)?("@#" + (*it)->getName()):("#" + (*it)->getName());
			_buffer += " ";
		}
		_buffer += "\n";
		_buffer += ":";
		_buffer += SERVER_NAME;
		_buffer += " 312 " + client->getUserName() + " " + target->getUserName();
		_buffer += SERVER_NAME;
		_buffer += " ";
		_buffer += IRC_NAME;
		_buffer += "\n";
		_buffer += ":";
		_buffer += SERVER_NAME;
		_buffer += " 312 " + client->getUserName() + " " + target->getUserName();
		_buffer += " End of /WHOIS list";
		}
		_buffer += "\n";
	}
	_buffer += "\r\n";
	client->appendToResponse(_buffer);
	server->sendResponse(client);
}

void	Response::_noticeCmd(){
	if (client->getRegistered() == false)
		throw(std::runtime_error("Client not registred yet!"));
	std::vector<std::string> args = cmd.getArgs();
	_buffer.clear();
	if (args.size() < 2)
		return ;
	
	_buffer += ":" + client->getNickName() + "!" + client->getUserName() + "@" + SERVER_NAME + " ";
	if (args[0][0] == '#') {
		args[0].erase(0, 1);
		Channel *channel = manager.find(args[0]);
		if (channel == NULL)
			return ;
		std::set<Client *> &members = channel->getMembers();
		_buffer += "NOTICE #" + args[0] + " ";
		if (args.size() == 2 && args[1].size() == 1 && args[1][0] == ':')
			_buffer += ":";
		else if (args[1][0] != ':')
			_buffer += args[1];
		else{
			for (size_t i = 1; i < args.size(); i++){
				_buffer += args[i];
				if (i + 1 != args.size())
					_buffer += " ";
			}
		}
		_buffer += "\r\n";
		for (std::set<Client*>::iterator it = members.begin(); it != members.end(); it++)
		{
			(*it)->appendToResponse(_buffer);
			server->sendResponse(*it);
		}
	}
	else {
		Client *target = server->getClientByName(args[0]);
		if (target == NULL)
			return ;
		_buffer += args[0] + " ";
		if (args.size() == 2 && args[1].size() == 1 && args[1][0] == ':')
			_buffer += ":";
		else if (args[1][0] != ':')
			_buffer += args[1];
		else{
			for (size_t i = 1; i < args.size(); i++){
				_buffer += args[i];
				if (i + 1 != args.size())
					_buffer += " ";
			}
		}
		_buffer += "\r\n";
		target->appendToResponse(_buffer);
		server->sendResponse(target);
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
	else if (cmd.getType() == MODE)
		_modeCmd();
	else if (cmd.getType() == UNKNOWN)
		_unknownCmd();
	else if (cmd.getType() == NAMES)
		_namesCmd();
	else if (cmd.getType() == PART)
		_partCmd();
	else if (cmd.getType() == WHOIS)
		_whoisCmd();
	else if (cmd.getType() == NOTICE)
		_noticeCmd();
	else {
		std::cout << "Type is : " << cmd.getType() << std::endl;
		throw(std::runtime_error("(" + cmd.getName() + ")" + ": Not implemented yet!"));
	}
}
