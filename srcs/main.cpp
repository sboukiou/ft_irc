#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include <cstdio>


int main(int ac, char **av) {
	Server server;

	server.run();
	return (0);
}
