#include "Server.hpp"

Server::Server(int port, std::string password): _port(port), _password(password){
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
}

void Server::initSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
        throw std::runtime_error("Error: Socket failed");
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(_socket, (sockaddr *)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Error: Socket failed");

    if (listen(_socket, 1000) < 0)
        throw std::runtime_error("Error: Socket failed");
}

void Server::acceptClient()
{
    struct sockaddr_in client_info;
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
    pollfds.push_back(info);
    Client *client = new Client(client_fd);
    Clients[client_fd] = client;
}

void Server::removeClient(Client *client)
{
    std::cerr << "Error: remove client fd=" << client->getFd() << std::endl;
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

void Server::handleRequest(pollfd info)
{
    if (info.revents & (POLLERR || POLLHUP))
    {   
        if (info.fd == _socket)
            std::cerr << "Error: client couldn't connect\n";
        else
            removeClient(Clients[info.fd]);
        return ;
    }
    if (info.fd == _socket && info.revents)
        acceptClient();
    else
    {
        
    }


}

void Server::run()
{
    while (true)
    {
        if (poll(&pollfds[0], pollfds.size(), -1) < 0)
            throw std::runtime_error("Error: poll failed");
        for (size_t i = 0; i < pollfds.size(); i++)
            handleRequest(pollfds[i]);


    }
}