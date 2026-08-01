#ifndef RESPONSE_HPP
# define  RESPONSE_HPP

#include "./macros.hpp"
#include "./Command.hpp"
#include "Client.hpp"
#include "ChannelManager.hpp"
#include "Server.hpp"


class Response {
	private:
		std::string _buffer;
		Command cmd;
		Client *client;
		std::string _password;
		ChannelManager manager;
		Server		*server;
		void	_nickNameCmd();
		void	_helpCmd();
		void	_quitCmd();
		void	_userCmd();
		void	_passCmd();
		void	_pingCmd();
	public:
		Response();
		Response(Command &cmd, Client *cl, std::string password, ChannelManager& manager, Server *server);
		Response(const Response &other);
		Response &operator=(const Response &other);
		std::string getBuffer(void) const;
		void	runCmd();
		~Response();
};

#endif /* RESPONSE_HPP */
