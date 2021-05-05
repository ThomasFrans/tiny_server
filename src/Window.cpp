#include "Window.h"
#include <iomanip>
#include <iostream>

bool Tiny::Window::send_message(Gdk::Event* event)
{
    spdlog::info("Key pressed");
    // Glib::RefPtr<Tiny::Application> app = get_application();
    // Tiny::Application *app_ptr = app.get();
    // _callback_fun(app_ptr, message);
    return true;
}

Tiny::Window::Window(/* args */)
{

    _builder = Gtk::Builder::create_from_file("/home/thomas/Documents/Programming/tiny_chat/tiny_server/res/Window.glade");
    _builder->get_widget("box_layout", _layout);
    _builder->get_widget("button_send", _button_send);
    _builder->get_widget("textview_messages", _textview_messages);
    _builder->get_widget("entry_message", _entry_message);
    _textview_messages_buffer = _textview_messages->get_buffer();
    set_default_geometry(720, 480);
    add(*_layout);
    present();
}

Tiny::Window::~Window()
{
    delete _layout;
}

void Tiny::Window::display_message(std::string message)
{
    std::string full_message("");
    full_message.append(message);
    full_message.append("\n");
    Glib::RefPtr<Gtk::TextMark> mark = _textview_messages_buffer->get_insert();
    _textview_messages_buffer->insert(mark->get_iter(), full_message.c_str());
}

void Tiny::Window::set_callback(Tiny::Application* app, void (*callback_fun)(Tiny::Application *app, std::string message))
{
    _app = app;
    _callback_fun = callback_fun;
}

bool Tiny::Window::on_key_press_event(GdkEventKey *event)
{
    if (event->keyval == GDK_KEY_Return)
    {
        spdlog::info("Enter pressed");
        Glib::ustring text = _entry_message->get_text();
        _entry_message->set_text("");
        _callback_fun(_app, std::string(text));
        return true;    // Event handled
    }
    return Gtk::ApplicationWindow::on_key_press_event(event);
}