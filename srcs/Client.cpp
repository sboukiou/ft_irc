#include "Client.hpp"

Client::Client(){

}
Client::Client(const Client& other){
    *this = other;
}
Client& Client::operator=(const Client& other){
    (void) other;
    return *this;
}
Client::~Client(){

}