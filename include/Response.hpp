#ifndef RESPONSE_HPP
# define  RESPONSE_HPP

#include "./macros.hpp"
#include "./Command.hpp"
#include <errno.h>
#include "Client.hpp"
#include "ChannelManager.hpp"
#include "Server.hpp"


class Response {
	private:
		std::string _buffer;
		Command cmd;
		Client *client;
		std::string _password;
		ChannelManager &manager;
		Server		*server;
		void	_nickNameCmd();
		void	_helpCmd();
		void	_quitCmd();
		void	_userCmd();
		void	_passCmd();
		void	_joinCmd();
		void	_kickCmd();
		void	_pingCmd();
		void	_clearCmd();
		void	_unknownCmd();
		void 	_inviteCmd();
		void	_topicCmd();
		void	_modeCmd();
		void	_privMsgCmd();
		void	_listCmd();
		void	_whoCmd();
		void	_namesCmd();
		void	_partCmd();
		void	_broadCastToChannel(Channel *chan, std::string message, bool includeSender);
		void	_whoisCmd();
		void	_noticeCmd();
	public:
		Response(Command &cmd, Client *cl, std::string password, ChannelManager& manager, Server *server);
		Response(const Response &other);
		Response &operator=(const Response &other);
		std::string getBuffer(void) const;
		void	runCmd();
		~Response();
};

#endif /* RESPONSE_HPP */
