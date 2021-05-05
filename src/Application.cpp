#include "Application.h"
#include "Window.h"
#include <thread>
#include "Server.h"
#include <iostream>

Tiny::Application::Application(/* args */)
{
    window = new Tiny::Window();
    Server server("localhost", 18550);
    server.bind();
    server.listen();
    server.set_callback(this, &Application::message_callback);
    client = server.accept();
    window->set_callback(this, &Application::window_callback);
    run(*window);
    spdlog::info("Server stopped");
    exit(EXIT_SUCCESS);
}

Tiny::Application::~Application()
{
    delete window;
    delete client;
}

void Tiny::Application::message_callback(Tiny::Application* app, std::string message)
{
    app->window->display_message(message);
}

void Tiny::Application::window_callback(Tiny::Application* app, std::string message)
{
    spdlog::info("Message received");
    app->client->send_message(message.c_str());
    app->window->display_message(message);
}