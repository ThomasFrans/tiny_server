#include "Server.h"

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <memory>
#include <arpa/inet.h>
#include <string>
#include <iostream>
#include <thread>

Tiny::Server::Server(const char *address, int port) : _addrlen(sizeof(_sock_address))
{
    int opt = 1;
    clients = new std::map<std::string, Client>;
    connections = new std::map<std::string, Client>;
    // Creating socket file descriptor
    if ((_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    _sock_address.sin_family = AF_INET;
    if (inet_pton(AF_INET, "192.168.68.139", &_sock_address.sin_addr.s_addr) <= 0)
    {
        perror("Error");
        exit(EXIT_FAILURE);
    }
    _sock_address.sin_port = htons(port);
}

bool Tiny::Server::bind()
{

    // Forcefully attaching socket to the port 8080
    if (::bind(_fd, (struct sockaddr *)&_sock_address,
               sizeof(_sock_address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return true;
}

bool Tiny::Server::listen()
{

    if (::listen(_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return true;
}

void Tiny::Server::start()
{
    _client_listener = new std::thread(&Server::client_listener, *this);
}

void Tiny::Server::client_listener()
{
    int socket;
    while (true)
    {
        if ((socket = ::accept(_fd, (struct sockaddr *)&_sock_address,
                               (socklen_t *)&_addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        handle_client(socket);
    }
}

void Tiny::Server::handle_client(int socket)
{
    send_socket(socket, "Hello, please enter a name: ");
    std::string buffer = receive_socket(socket);
    std::string welcome_message = "User ";
    welcome_message.append(buffer);
    welcome_message.append(" now registered!");
    send_socket(socket, welcome_message.c_str());
    spdlog::info("Got client socket");
    (*clients)[std::string(buffer)].socket = socket;
    spdlog::info("Added Client socket");
    std::thread *read_thread = new std::thread(&Server::read, *this, std::string(buffer));
    spdlog::info("Added read thread");
    (*clients)[std::string(buffer)].read_thread = read_thread;
    spdlog::info("Completed client add");
}

void Tiny::Server::write(std::string message)
{

    std::string l_message;
    while (true)
    {
        getline(std::cin, l_message);
        // client->send_message(l_message.c_str());
    }
}

void Tiny::Server::read(std::string client)
{
    int valread;
    while (true)
    {
        char buffer[1024] = {0};
        valread = recv(clients->at(client).socket, buffer, 1024, 0);
        if (valread == 0)
        {
            exit(EXIT_SUCCESS);
        }
        process_message(client, buffer);
    }
}

void Tiny::Server::process_message(std::string client, const char *message)
{
    std::size_t find;
    std::string s_message(message);
    find = s_message.find(" ");
    if (s_message.find("!connect") != std::string::npos)
    {
        (*connections)[client] = clients->at(s_message.substr(find + 1));
        (*connections)[s_message.substr(find + 1)] = clients->at(client);
    } else if (connections->count(client) > 0) {
        send_socket(connections->at(client).socket, message);
    }
    else
    {
        spdlog::info(message);
    }
}

void Tiny::Server::send_socket(int socket, const char *message)
{
    send(socket, message, strlen(message), 0);
}

std::string Tiny::Server::receive_socket(int socket)
{
    char buffer[1024] = {0};
    recv(socket, buffer, 1024, 0);
    return std::string(buffer);
}

void Tiny::Server::set_callback(Tiny::Application *s, void (*fp)(Tiny::Application *, std::string))
{
    app = s;
    callback_fun = fp;
}

Tiny::Server::~Server()
{
    _client_listener->join();
    delete _client_listener;
    delete connections;
    delete clients;
}