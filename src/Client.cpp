#include "Client.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Tiny::Client::Client(Server *server)
{
    _server = server;
    if ((_socket = ::accept(server->_fd, (struct sockaddr *)&_server->_sock_address,
                          (socklen_t *)&server->_addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
}

void Tiny::Client::send_message(const char *message)
{
    send(_socket, message, strlen(message), 0);
}

int Tiny::Client::get_socket()
{
    return _socket;
}