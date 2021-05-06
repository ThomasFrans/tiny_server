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
#include <chrono>

std::mutex clients_mutex;

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
    client_listener();
}

void Tiny::Server::client_listener()
{
    int socket;
    std::vector<std::thread> threads;
    while (true)
    {
        if ((socket = ::accept(_fd, (struct sockaddr *)&_sock_address,
                               (socklen_t *)&_addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        threads.push_back(std::thread(&Server::handle_client, socket, this));
        if (threads.size() > 3)
        {
            std::vector<std::thread>::iterator it;
            while (threads.size() > 2)
            {
                for (it = threads.begin(); it < threads.end(); it++)
                {
                    if (it->joinable())
                    {
                        it->join();
                        threads.erase(it);
                    }
                }
            }
        }
    }
}

void Tiny::Server::handle_client(int socket, Server *s)
{
    Client temp(socket);
    json welcome_msg;
    welcome_msg["command"] = "Hello, please enter a name";
    s->write(temp, welcome_msg);
    json response = s->read(socket, s);
    std::string welcome_message = "User ";
    welcome_message.append(response["name"].get<std::string>());
    welcome_message.append(" now registered!");
    json hello_msg;
    hello_msg["command"] = welcome_message;
    s->write(temp, hello_msg);

    Client *client = new Client();
    client->name = response["name"].get<std::string>();
    client->socket = socket;
    client->connected = true;
    s->add_client(client);
    while (client->connected)
    {
        response = Tiny::Server::read(*client, s);
        spdlog::info("here now");
        if (client->connected)
        {
            response["client"] = client->name;
            s->command_dispatcher(response);
        }
    }
}

void Tiny::Server::write(Client &client, json msg)
{

    std::string buffer = msg.dump();
    const char* c_buffer = buffer.c_str();
    send(client.socket, c_buffer, strlen(c_buffer), 0);
}

json Tiny::Server::read(Client &client, Server *s)
{
    char buffer[1024] = {0};
    json res;
    if (recv(client.socket, buffer, 1024, 0) == 0)
    {
        close(client.socket);
        s->remove_client(client.name);
        if (s->clients->size() == 0)
        {
            exit(EXIT_SUCCESS);
        }
        client.connected = false;
        res["something"] = "somethingelse";
    }
    else
    {
        res = json::parse(buffer);
    }
    return res;
}

json Tiny::Server::read(int socket, Server *s)
{
    char buffer[1024] = {0};
    if (recv(socket, buffer, 1024, 0) == 0)
    {
        close(socket);
        exit(EXIT_FAILURE); // TODO exit gracefully when a client exits before full client creation!
    }
    else
    {
        return json::parse(buffer);
    }
}

void Tiny::Server::command_dispatcher(json packet)
{
    if (packet["command"].get<std::string>() == std::string("connect"))
    {
        spdlog::info("connect");
    }
    else if (packet["command"].get<std::string>() == std::string("disconnect"))
    {
        spdlog::info("disconnect");
    }
    else if (packet["command"].get<std::string>() == std::string("list"))
    {
        spdlog::info("list");
        std::vector<std::string> members;
        for (std::map<std::string, Client>::iterator it = clients->begin(); it != clients->end(); it++)
        {
            members.push_back(it->first);
        }
        json res;
        res["body"] = members;
        res["command"] = "members";
        write(clients->at(packet["client"].get<std::string>()), res);
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

void Tiny::Server::add_client(Client *client)
{
    clients_mutex.lock();
    (*clients)[client->name] = *client;
    clients_mutex.unlock();
}

void Tiny::Server::remove_client(std::string client)
{
    remove_connection(client);
    spdlog::info("All connections removed");
    // delete client.read_thread;
    clients_mutex.lock();
    clients->erase(client);
    clients_mutex.unlock();
    spdlog::info("Client removed");
}

void Tiny::Server::add_connection(std::string first, std::string second)
{
    clients_mutex.lock();
    spdlog::info("at here");
    (*connections)[first] = clients->at(second);
    (*connections)[second] = clients->at(first);
    clients_mutex.unlock();
}

void Tiny::Server::remove_connection(std::string client)
{
    if (connections->count(client) > 0)
    {
        spdlog::info("at here 3");
        connections->erase(connections->at(client).name);
        connections->erase(client);
    }
}