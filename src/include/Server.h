#pragma once

#include "Tiny.h"
#include <netinet/in.h>
#include <memory>
#include <string>
#include <string.h>
#include "Application.h"
#include <thread>
#include <map>

namespace Tiny
{

    class Application;
    
    class Server
    {
    private:
        struct Client
        {
            int socket;
            std::thread* read_thread;
        };
        
        std::map<std::string, Client> *clients;
        std::map<std::string, Client> *connections;
        const char* address;
        int port, _fd, _addrlen;
        struct sockaddr_in _sock_address;
        std::thread *_client_listener;
        Tiny::Application* app;
        void (*callback_fun)(Tiny::Application*, std::string);
        void send_socket(int, const char*);
        std::string receive_socket(int);
        void handle_client(int);
        void client_listener();
        void process_message(std::string client, const char* message);
    public:
        
        Server(const char* address, int port);
        ~Server();
        bool bind();
        bool listen();
        void start();
        void write(std::string message);
        void read(std::string client);
        void set_callback(Tiny::Application* s, void (*fp)(Tiny::Application*, std::string));
    };
} // namespace Tiny

