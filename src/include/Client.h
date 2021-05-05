#pragma once

#include "Tiny.h"
#include "Server.h"

namespace Tiny
{
    class Server;
    
    class Client
    {
    private:
        int _socket;
        Server* _server;
    public:
        Client(Server *server);
        void send_message(const char* message);
        int get_socket();
    };
} // namespace Tiny