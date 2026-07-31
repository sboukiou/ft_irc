#ifndef RESPONSE_HPP
# define  RESPONSE_HPP

#include "./macros.hpp"
#include "./Command.hpp"
#include "Client.hpp"


class Response {
	private:
		std::string _buffer;
		Command cmd;
		Client *client;
		std::string _password;
		void	_nickNameCmd();
		void	_helpCmd();
		void	_quitCmd();
		void	_userCmd();
		void	_passCmd();
	public:
		Response();
		Response(Command &cmd, Client *cl, std::string password);
		Response(const Response &other);
		Response &operator=(const Response &other);
		std::string getBuffer(void) const;
		void	runCmd();
		~Response();
};

#endif /* RESPONSE_HPP */
