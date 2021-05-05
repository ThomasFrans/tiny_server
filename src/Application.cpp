#include "Application.h"
#include <thread>
#include "Server.h"
#include <iostream>

Tiny::Application::Application(/* args */)
{
    Server server("localhost", 18550);
    server.bind();
    server.listen();
    server.set_callback(this, &Application::message_callback);
    std::thread th(&Server::start, server);
    // spdlog::info("Starting GUI");
    th.join();
    spdlog::info("Server stopped");
    exit(EXIT_SUCCESS);
}

Tiny::Application::~Application()
{

}

void Tiny::Application::message_callback(Tiny::Application* app, std::string message)
{
    
}

void Tiny::Application::window_callback(Tiny::Application* app, std::string message)
{

}