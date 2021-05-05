#pragma once

#include "Tiny.h"
#include <netinet/in.h>
#include "Client.h"
#include <memory>
#include <string>
#include "Application.h"
#include <thread>

namespace Tiny
{
    class Client;

    class Application;
    
    class Server
    {
    private:
        const char* address;
        int port, _fd, _addrlen;
        friend class Client;
        struct sockaddr_in _sock_address;
        std::thread *read_thread;
        std::thread *write_thread;
        Tiny::Application* app;
        void (*callback_fun)(Tiny::Application*, std::string);
        
    public:
        
        Server(const char* address, int port);
        ~Server();
        bool bind();
        bool listen();
        Tiny::Client* accept();
        void write(Client* client, std::string message);
        void read(Client* client);
        void set_callback(Tiny::Application* s, void (*fp)(Tiny::Application*, std::string));
    };
} // namespace Tiny

