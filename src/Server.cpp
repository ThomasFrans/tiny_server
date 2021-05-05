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

Tiny::Server::Server(const char *address, int port)
{
    int new_socket, valread;

    int opt = 1;
    _addrlen = sizeof(_sock_address);
    char buffer[1024] = {0};
    const char *hello = "Hello from server";

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

Tiny::Client* Tiny::Server::accept()
{
    Client *client = new Client(this);
    read_thread = new std::thread(&Server::read, *this, client);
    return client;
}

void Tiny::Server::write(Client *client, std::string message)
{

    std::string l_message;
    while (true)
    {
        getline(std::cin, l_message);
        client->send_message(l_message.c_str());
    }
}

void Tiny::Server::read(Client *client)
{
    int valread;
    while (true)
    {
        char buffer[1024] = {0};
        valread = recv(client->get_socket(), buffer, 1024, 0);
        if (valread == 0)
        {
            exit(EXIT_SUCCESS);
        }
        callback_fun(app, std::string(buffer));
    }
}

void Tiny::Server::set_callback(Tiny::Application* s, void (*fp)(Tiny::Application*, std::string))
{
    app = s;
    callback_fun = fp;
}

Tiny::Server::~Server()
{
    read_thread->join();
    delete read_thread;
}