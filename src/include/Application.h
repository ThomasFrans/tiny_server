#pragma once

#include "Tiny.h"
#include <string>

#include "Window.h"

namespace Tiny 
{

    class Window;

    class Application : public Gtk::Application
    {
    private:
    public:
        Tiny::Window *window;
        Application(/* args */);
        ~Application();
        static void message_callback(Tiny::Application* app, std::string message);
        void send_message(std::string message);
        static void window_callback(Tiny::Application* app, std::string message);
    };
} // namespace Tiny
