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