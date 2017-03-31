#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>
//#include "../../src/ring2.h"

namespace jetdb{
	namespace handlers{

		int bookFlightByID(pqxx::transaction_base& txn, std::string clientGovID, std::vector<std::string>
		otherPassengerGovIDs,
						   std::vector<std::string> flightIDList)
		{
			std::string bid;
			std::string flightTotalCost;
			std::time_t unixTime = std::time(0);


			if(clientGovID == "" || flightIDList.size() < 1)
			{
				return -1; //error
			}

			try
			{
				//get flight total cost
				std::string flightCostQuery = "SELECT SUM(Cost) FROM Flight WHERE fid IN (";
				std::string fidConcatString;
				for(int i = 0; i < flightIDList.size() - 1; i++)
				{
					fidConcatString += txn.quote(flightIDList[i]) + ", ";
				}

				flightCostQuery += fidConcatString +
								   txn.quote(flightIDList[flightIDList.size() - 1]) + ");";

				pqxx::result result = txn.exec(flightCostQuery);
				flightTotalCost = result[0][0].c_str();

				//insert into booking table
				std::string bookingInsert = "INSERT INTO booking (bid) VALUES (DEFAULT) RETURNING bid";
				result = txn.exec(bookingInsert);
				bid = result[0][0].c_str();

				//insert into books table
				txn.conn().prepare(
						"booksInsert",
						"INSERT INTO books (govid, bid, datebooked, cost) VALUES ($1, $2, to_timestamp($3), $4);");

				result = txn.prepared("booksInsert")(clientGovID)(bid)(unixTime)(
						flightTotalCost).exec(); //handle client

				for(unsigned long i = 0; i < otherPassengerGovIDs.size(); i++)
				{
					result = txn.prepared("booksInsert")(otherPassengerGovIDs[i])(bid)(unixTime)(
							flightTotalCost).exec();
				}

//	insert into booked table
				txn.conn().prepare(
						"bookedInsert",
						"INSERT INTO booked (bid, fid) VALUES ($1, $2);");
				for(unsigned long i = 0; i < flightIDList.size(); i++)
				{
					result = txn.prepared("bookedInsert")(bid)(flightIDList[i]).exec();
				}
			} catch(const pqxx::pqxx_exception &e)
			{
				return -2; //some exception thrown
			}

			return 0; //all good
		}

		namespace bookFlight{
			responses::result successMsg = {true, "successfully booked", nullptr};
			responses::result failureMsg = {false, "invalid booking", nullptr};

		}

		responses::result handle_request(pqxx::work& txn, requests::bookFlight req)
		{
			int result = bookFlightByID(txn,
										req.clientGovID,
										req.otherPassengerGovIDs,
										req.flightIDList);

			if(result == 0)
			{
				return jetdb::handlers::bookFlight::successMsg;
			}
			return jetdb::handlers::bookFlight::failureMsg;
		}
	}
}
