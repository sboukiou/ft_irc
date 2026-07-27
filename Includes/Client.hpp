#ifndef Client_hpp
#define Client_hpp

#include <iostream>

class Client{
    private:
        int _fd;
        std::string buffer;
        bool is_complete;

    public:
        Client();
        Client(int fd);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        void setIsCoplete(bool status);
        bool getIsComplete() const;
        void setBuffer(std::string buf);
        std::string getBuffer() const;
        int getFd() const;
};


#endif