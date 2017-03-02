//
// Created by eric on 2/21/17.
//
#include <iostream>
#include <pqxx/pqxx>
#include "ring2.h"
#include "ring2Window.h"
#include <gtkmm.h>

int main(int argc, char** argv)
{
	try
	{
		pqxx::connection c("dbname=postgres user=postgres password= hostname=localhost");
		std::string loopStatus;

		Gtk::Main kit(argc, argv);

		ring2Window example;

		Gtk::Main::run(example);



		do //book stuff
		{
			printf("Booking a flight (enter 'n' to quit)\n");
			printf("Enter '?' to lookup acceptable input\n");
			std::tuple<std::vector<std::string>, std::vector<std::string>> userInfo = getBookingInfo(c);

			int bookingStatus = bookFlightByID(c, userInfo);

			printf("booking status: %d\n", bookingStatus);

			printf("Thank you\n\nBook another flight?\n");
			std::getline(std::cin, loopStatus);
		} while(loopStatus != "n");
	} catch(const std::exception& e)
	{
		std::cout << "DB connection error\n";
	}

}