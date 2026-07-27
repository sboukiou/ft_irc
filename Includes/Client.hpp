#ifndef Client_hpp
#define Client_hpp

class Client{
    
    public:
        Client();
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();
};


#endif