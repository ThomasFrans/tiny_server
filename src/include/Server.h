#pragma once

#include "Tiny.h"
#include <netinet/in.h>
#include <memory>
#include <string>
#include <string.h>
#include "Application.h"
#include <thread>
#include <map>
#include <mutex>
#include <cereal/archives/json.hpp>

namespace Tiny
{

    class Application;

    class Server
    {
    private:
        struct Client
        {
            int socket;
            bool connected;
            std::string name;

            Client() = default;
            Client(int socket) : socket(socket) {};
        };

        struct Command
        {
            std::string command;
            std::string argument;

            Command(std::string com, std::string arg) : command(com), argument(arg){};

            template <class Archive>
            void serialize(Archive &archive)
            {
                archive(command, argument); // serialize things by passing them to the archive
            }
        };

        std::map<std::string, Client> *clients;
        std::map<std::string, Client> *connections;
        const char *address;
        int port, _fd, _addrlen;
        struct sockaddr_in _sock_address;
        std::thread *_client_listener;
        std::thread *_client_disposer;
        Tiny::Application *app;
        void (*callback_fun)(Tiny::Application *, std::string);
        void send_socket(int, const char *);
        std::string receive_socket(int);
        static void handle_client(int, Server *);
        void client_listener();
        void add_client(Client *);
        void remove_client(std::string);
        void remove_connection(std::string);
        void add_connection(std::string, std::string);
        void command_dispatcher(json);

    public:
        Server(const char *address, int port);
        ~Server();
        bool bind();
        bool listen();
        void start();
        void write(Client&, json);
        static json read(Client &client, Server *s);
        static json read(int socket, Server *s);
        void set_callback(Tiny::Application *s, void (*fp)(Tiny::Application *, std::string));
    };
} // namespace Tiny
