#include "Bot.hpp"
#include <iostream>
#include <unistd.h> // sleep()

int main()
{
    try
    {
        Bot bot(3030, "127.0.0.1", "1234", "Bot", "Hisoka", "OussamaBot");

        if (bot.connectServer())
        {
            std::cout << "Connected to server successfully!" << std::endl;

            // Keep the connection open for 10 seconds
            sleep(10);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}