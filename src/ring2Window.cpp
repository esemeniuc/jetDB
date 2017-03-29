//
// Created by eric on 3/1/17.
//

#include <iostream>
#include "ring2Window.h"


//from wikipedia gtkmm page
ring2Window::ring2Window()
: button1("testing button")
{
	set_title("its a title");
	resize(480, 640);
	set_border_width(10);

//	button1.set_property("width-request", 40);
//	button1.set_property("height-request", 40);
	add(button1);

	button1.signal_clicked().connect([]() {std::cout << "clicked that button\n";});
	struct tmp{
		Gtk::Button const& s;
		tmp(Gtk::Button& b): s{b}{}
		void operator()(){
			//...
		}
	};
	/*button1.signal_clicked().connect([&s=button1]() {
		//...
	});*/
	button1.signal_clicked().connect(tmp{button1});
	show_all_children();
}