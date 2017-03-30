//
// Created by eric on 2/21/17.
//
#include <iostream>
#include <pqxx/pqxx>
#include "ring2.h"

int main(int argc, char** argv)
{
	pqxx::connection c("dbname=jetdb user=postgres password= hostname=localhost");
	std::string loopStatus;

	//book stuff

	do
	{
		printf("Booking a flight (enter 'n' to quit)\n");
		printf("Enter '?' to lookup acceptable input\n");
		std::tuple<std::vector<std::string>, std::vector<std::string>> userInfo = getBookingInfo(c);

		int bookingStatus = bookFlightByID(c, userInfo);

		printf("booking status: %d\n", bookingStatus);

		printf("Thank you\n\nBook another flight?\n");
		std::getline(std::cin, loopStatus);
	} while(loopStatus != "n");
}