#include "../include/Server.hpp"
#include "../include/macros.hpp"


int main(int ac, char **av) {
	int	port;
	std::string password;

	if (ac != 3) {
		std::cerr << "Invalid number of arguments!\n";
		return (0);
	}
	port = atoi(av[1]);
	password = std::string(av[2]);
	Server server(port, password);

	LOG("Launching the server");
	try {
		server.run();
	}
	catch (std::exception &e) {
		ERR(e.what());
		// TODO: Implement the cleanup method for the server
		// server.cleanup();
		return (0);
	}
	return (0);
}
