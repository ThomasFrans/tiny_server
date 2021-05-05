#pragma once

#include "Tiny.h"
#include "Application.h"
#include <string>

namespace Tiny
{
    class Application;

    class Window : public Gtk::ApplicationWindow
    {
    private:
        Tiny::Application *_app;
        Glib::ustring *text;
        Glib::RefPtr<Gtk::Builder> _builder;
        Gtk::Widget *_layout;
        Gtk::Button *_button_send;
        Gtk::Entry *_entry_message;
        Gtk::TextView *_textview_messages;
        Glib::RefPtr<Gtk::TextBuffer> _textview_messages_buffer;
        void (*_callback_fun)(Tiny::Application *app, std::string message);
        bool on_key_press_event(GdkEventKey *event) override;

    public:
        bool send_message(Gdk::Event *event);
        Window(/* args */);
        ~Window();
        void display_message(std::string message);
        void set_callback(Tiny::Application *app, void (*callback_fun)(Tiny::Application *app, std::string message));
    };

} // namespace Tiny
