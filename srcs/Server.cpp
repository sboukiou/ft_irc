#include "../include/Server.hpp"

Server::Server(int port, std::string password):_socket(-1), _port(port), _password(password){
}
Server::Server():_socket(-1), _port(-1){
}
Server::Server(const Server& other){
    *this = other;
}
Server& Server::operator=(const Server& other){
    (void)other;
    return *this;
}
Server::~Server(){
    if (_socket >= 0)
        close(_socket);
    for (std::map<int, Client*>::iterator it = Clients.begin(); it != Clients.end(); it++)
        delete it->second;
    Clients.clear();
}

void Server::initSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
        throw std::runtime_error("Error: Socket failed");
    int opt = 1;;
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
        throw std::runtime_error("Error");

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_socket, (sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Error: bind failed");

    if (listen(_socket, 1000) < 0)
        throw std::runtime_error("Error: listen failed");
    pollfd info;
    info.fd = _socket;
    info.events = POLLIN;
    info.revents = 0;
    pollfds.push_back(info);
}

void Server::acceptClient()
{
    sockaddr_in client_info = {};
    socklen_t client_len = sizeof(client_info);

    int client_fd = accept(_socket, (struct sockaddr *)&client_info, &client_len);
    if (client_fd < 0)
    {
        std::cerr << "Error: accept failed\n";
        return ;
    }
    pollfd info;
    info.fd = client_fd;
    info.events = POLLIN;
    info.revents = 0;
    pollfds.push_back(info);
    Client *client = new Client(client_fd);
    Clients[client_fd] = client;
}

void Server::removeClient(Client *client)
{
    Clients.erase(client->getFd());
    for (std::vector<pollfd>::iterator it = pollfds.begin(); it != pollfds.end(); it++)
    {
        if (client->getFd() == it->fd)
        {
            pollfds.erase(it);
            break;
        }
    }
    delete client;
}

void Server::extractCommand(Client *client)
{
    size_t pos;
    while ((pos = client->getBuffer().find("\n")) != std::string::npos && !client->getDisconnected())
    {
		/* TODO: Implement this */
        size_t commandEnd = pos;
        if (commandEnd > 0 && client->getBuffer()[commandEnd - 1] == '\r')
            commandEnd--;
        executeCommand(client, client->getBuffer().substr(0, commandEnd));
        client->getBuffer().erase(0, pos + 1);
    }
    if (client->getDisconnected())
        removeClient(client);
}

int Server::readRequest(Client *client)
{
    char buffer[1024];
    ssize_t read = recv(client->getFd(), buffer, sizeof(buffer), 0);
    if (read < 0)
    {
        std::cerr << "Error: recv failed\n";
        removeClient(client);
        return 1;
    }
    if (read == 0)
    {
        std::cerr << "Client Disconnected\n";
        removeClient(client);
        return 1;
    }
    client->appendBuffer(std::string(buffer, read));
    extractCommand(client);
    return 0;
}

int Server::sendResponse(Client *client)
{
    ssize_t bytesend = send(client->getFd(), client->getResponse().c_str(), client->getResponse().size(), 0); 
    if (bytesend < 0)
    {
        removeClient(client);
        return 1;
    }
    client->getResponse().erase(0, bytesend);
    return 0;
}

void Server::handleRequest(pollfd &info)
{
    if (info.revents == 0)
    {
        index++;
        return ;
    }
    if (info.revents & (POLLERR | POLLHUP))
    {   
        if (info.fd == _socket)
            std::cerr << "Error: client couldn't connect\n";
        else
        {
            std::map<int, Client*>::iterator it = Clients.find(info.fd);
            if (it != Clients.end())
                removeClient(it->second);
        }
        return ;
    }
    if (info.fd == _socket && (info.revents & POLLIN))
    {
        acceptClient();
        index++;
        return ;
    }
    if (info.revents & POLLIN)
    {
        std::map<int, Client*>::iterator it = Clients.find(info.fd);
        if (it != Clients.end())
        {
            if (readRequest(it->second))
                return ;

        }
    }
    if (info.revents & POLLOUT)
    {
        if (sendResponse(Clients[info.fd]))
            return ;
    }
    if (Clients[info.fd]->getResponse().size() > 0)
        info.events |= POLLOUT;
    else
        info.events &= ~POLLOUT;
    index++;
}

void Server::run()
{
    initSocket();

    while (true)
    {
        if (poll(&pollfds[0], pollfds.size(), -1) < 0)
            throw std::runtime_error("Error: poll failed");
        index = 0;
        while (index < pollfds.size())
            handleRequest(pollfds[index]);
    }
}


void	Server::executeCommand(Client *client, std::string command) {
	if (client == NULL)
		throw(std::runtime_error("Client [NULL] sent the command: " + command));
	else {
		std::string response;
		response += "Client: ";
		response += client->getNickname();
		response += "Has requesed the command: " ;
		response += command;
		client->appendToResponse(response);
	}
}
