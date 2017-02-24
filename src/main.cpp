//
// Created by eric on 2/21/17.
//
#include <iostream>
#include <pqxx/pqxx>
#include "ring2.h"

int main(int argc, char** argv)
{
	pqxx::connection c("dbname=postgres user=postgres password= hostname=localhost");
	pqxx::work txn(c);


	std::tuple<std::string, std::string, std::vector<std::string>> userInfo = getBookingInfo(txn);

	int bookingStatus = bookFlightByID(txn, userInfo);
}