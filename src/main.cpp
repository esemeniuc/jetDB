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


	int userInfo = getBookingInfo(txn);

}