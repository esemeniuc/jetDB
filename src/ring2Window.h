//
// Created by eric on 3/1/17.
//

#ifndef JETDB_RING2WINDOW_H
#define JETDB_RING2WINDOW_H


#include <gtkmm/window.h>
#include <gtkmm/button.h>

class ring2Window : public Gtk::Window
{

public:
	ring2Window();

protected:
	Gtk::Button button1;
};


#endif //JETDB_RING2WINDOW_H
