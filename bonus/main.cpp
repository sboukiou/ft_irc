#include "Bot.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>\n";
        return 1;
    }
    if (!digits(av[1]))
    {
        std::cerr << "please enter a valid port\n";
        return 1;
    }
    try
    {
        
        int port = std::atoi(av[1]);
        std::string pass = av[2];
        Bot bot(port, "127.0.0.1", pass,
                "ircbot", "ircbot", "Simplebot");

        if (bot.connectServer())
        {
            
            bot.authenticate();
            int connected = 1;
            while (bot.receive())
            {
                if (connected == 1)
                    std::cout << "Connected to server successfully!\n";
                connected = -1;
            }
            
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

}