//
// Created by eric on 2/18/17.
//
#include <cstdio>
#include <ctime>
#include <iostream>
#include "ring2.h"
#include "utility.h"

//preconditions: GovIDs and flight IDs in the db
//description: use to book a flight after acquiring all valid user info from interactive prompt or GUI
//postconditions: returns 0 on successful booking, nonzero otherwise
int bookFlightByIDTuple(pqxx::connection_base& c, std::tuple<std::vector<std::string>, std::vector<std::string>>
userInfo)
{
	pqxx::work txn(c);
	std::string bid;
	std::string flightTotalCost;
	std::time_t unixTime = std::time(0);
	std::vector<std::string> allPassengerGovIDs;
	std::vector<std::string> flightIDList;
	std::tie(allPassengerGovIDs, flightIDList) = userInfo;

	//alternate unpack of data
	//get<0> = govIDs of all passengers
	//get<1> = flightID
//	std::vector<std::string>& allPassengerGovIDs = std::get<1>(userInfo);
//	std::string& flightID = std::get<1>(userInfo);

	if(allPassengerGovIDs.size() < 1 || flightIDList.size() < 1)
	{
		return -1; //error
	}

	//get flight total cost
	std::string flightCostQuery = "SELECT SUM(Cost) FROM Flight WHERE fid IN (";
	std::string idConcatString;
	for(int i = 0; i < flightIDList.size()-1; i++)
	{
		idConcatString += txn.quote(flightIDList[i]) + ", ";
	}

	flightCostQuery = flightCostQuery + idConcatString +
					  txn.quote(flightIDList[flightIDList.size()-1]) + ");";

	pqxx::result result = txn.exec(flightCostQuery);
	flightTotalCost = result[0][0].c_str();

	//insert into booking table
	std::string bookingInsert = "INSERT INTO booking (bid) VALUES (DEFAULT) RETURNING bid";
	result = txn.exec(bookingInsert);
	bid = result[0][0].c_str();

	//insert into books table
	c.prepare(
			"booksInsert",
			"INSERT INTO books (govid, bid, datebooked, cost) VALUES ($1, $2, to_timestamp($3), $4);");
	for(unsigned long i = 0; i < allPassengerGovIDs.size(); i++)
	{
		result = txn.prepared("booksInsert")(allPassengerGovIDs[i])(bid)(unixTime)(flightTotalCost).exec();
	}

//	insert into booked table
	c.prepare(
			"bookedInsert",
			"INSERT INTO booked (bid, fid) VALUES ($1, $2);");
	for(unsigned long i = 0; i < flightIDList.size(); i++)
	{
		result = txn.prepared("bookedInsert")(bid)(flightIDList[i]).exec();
	}

	txn.commit();
	return 0; //all good
}

int bookFlightByID(pqxx::connection_base& c, std::string clientGovID, std::vector<std::string> otherPassengerGovIDs,
				   std::vector<std::string> flightIDList)
{
	pqxx::work txn(c);
	std::string bid;
	std::string flightTotalCost;
	std::time_t unixTime = std::time(0);


	if(clientGovID == "" || flightIDList.size() < 1)
	{
		return -1; //error
	}

	//get flight total cost
	std::string flightCostQuery = "SELECT SUM(Cost) FROM Flight WHERE fid IN (";
	std::string fidConcatString;
	for(int i = 0; i < flightIDList.size()-1; i++)
	{
		fidConcatString += txn.quote(flightIDList[i]) + ", ";
	}

	flightCostQuery += fidConcatString +
					  txn.quote(flightIDList[flightIDList.size()-1]) + ");";

	pqxx::result result = txn.exec(flightCostQuery);
	flightTotalCost = result[0][0].c_str();

	//insert into booking table
	std::string bookingInsert = "INSERT INTO booking (bid) VALUES (DEFAULT) RETURNING bid";
	result = txn.exec(bookingInsert);
	bid = result[0][0].c_str();

	//insert into books table
	c.prepare(
			"booksInsert",
			"INSERT INTO books (govid, bid, datebooked, cost) VALUES ($1, $2, to_timestamp($3), $4);");

	result = txn.prepared("booksInsert")(clientGovID)(bid)(unixTime)(flightTotalCost).exec(); //handle client

	for(unsigned long i = 0; i < otherPassengerGovIDs.size(); i++)
	{
		result = txn.prepared("booksInsert")(otherPassengerGovIDs[i])(bid)(unixTime)(flightTotalCost).exec();
	}

//	insert into booked table
	c.prepare(
			"bookedInsert",
			"INSERT INTO booked (bid, fid) VALUES ($1, $2);");
	for(unsigned long i = 0; i < flightIDList.size(); i++)
	{
		result = txn.prepared("bookedInsert")(bid)(flightIDList[i]).exec();
	}

	txn.commit();
	return 0; //all good
}





//make query based on all the where conditions
//from, to airport code, based on the 2 date ranges
//returns the flight no
int getFlight(pqxx::work& txn,
			  std::string promptUserString,
			  std::vector<std::string>& flightIDList,
			  const std::string& fromAirport,
			  const std::string& toAirport,
			  const std::string& departDate,
			  const std::string& returnDate)
{
	std::string userInput;
	std::string subsequentPrompt = "Please flight code for additional flights (! to skip)";
	std::string lookupString = "SELECT fid, CONCAT(flight.prefix, flightname.flightnum) AS flightname, "
									   "airline.alname, flight.starttime, flight.endtime FROM flight "
									   "NATURAL JOIN named "
									   "NATURAL JOIN flightname "
									   "NATURAL JOIN airline "
									   "WHERE fromairportcode = " + txn.quote(fromAirport) +
							   " AND toairportcode = " + txn.quote(toAirport) +
							   " AND starttime >= " + txn.quote(departDate) +
							   " AND endtime <= " + txn.quote(returnDate);

	std::string validationString = "SELECT fid, CONCAT(flight.prefix, flightname.flightnum) AS flightname, "
			"airline.alname, flight.starttime, flight.endtime FROM flight "
			"NATURAL JOIN named "
			"NATURAL JOIN flightname "
			"NATURAL JOIN airline "
			"WHERE flight.fid = ";

	bool continueLoop = true;
	do //list all flights
	{
		if(flightIDList.size() == 0)
		{
			printf("%s\n", promptUserString.c_str());
		}
		else
		{
			printf("%s\n", subsequentPrompt.c_str());
		}

		std::getline(std::cin, userInput); //expecting in put for a flight id

		if(userInput == "?")
		{
			lookupPrint(lookupString, txn);
			continue; //don't run the rest
		}
		else if(userInput == "!" && flightIDList.size() > 0)
		{
			break;
		}

		pqxx::result validationResult = txn.exec(validationString + txn.quote(userInput));
		if(validationResult.size() != 1)
		{
			printf("Invalid entry, please try again\n");
		}
		else //assume valid
		{
			printResult(validationResult);
			flightIDList.push_back(userInput);

			//ask for more input
			printf("Add more flights to the booking? ('n' to skip)\n");
			std::getline(std::cin, userInput);
			if(userInput == "n")
			{
				continueLoop = false;
			}
		}
	} while(continueLoop);

	return (int)flightIDList.size();
}

int getGovIDs(pqxx::work& txn, std::string promptUserString, std::vector<std::string>& allPassengersGovID)
{
	std::string subsequentPrompt = "Please enter GovID for other passengers (! to skip)";

	std::string lookupString = "SELECT GovID, CName FROM Client";
	std::string validationString = ("SELECT GovID, CName "
			"FROM Client "
			"WHERE GovID = ");
	std::string userInput;
	pqxx::result validationResult;

	bool continueLoop = true;
	do
	{
		if(allPassengersGovID.size() == 0)
		{
			printf("%s\n", promptUserString.c_str());
		}
		else
		{
			printf("%s\n", subsequentPrompt.c_str());
		}

		std::getline(std::cin, userInput);

		if(userInput == "?")
		{
			lookupPrint(lookupString, txn);
			continue; //don't run the rest
		}
		else if(userInput == "!" && allPassengersGovID.size() > 0)
		{
			break;
		}

		validationResult = txn.exec(validationString + txn.quote(userInput));
		if(validationResult.size() != 1)
		{
			printf("Invalid entry, please try again\n");
		}
		else //assume valid
		{
			printResult(validationResult);
			allPassengersGovID.push_back(userInput);

			//ask for more input
			printf("Add more passengers to the booking? ('n' to skip)\n");
			std::getline(std::cin, userInput);
			if(userInput == "n")
			{
				continueLoop = false;
			}
		}
	}while(continueLoop);

	return (int)allPassengersGovID.size();
}


std::tuple<std::vector<std::string>, std::vector<std::string>> getBookingInfo(pqxx::connection_base& c)
{
	pqxx::work txn(c);
	std::vector<std::string> allPassengersGovID;
	std::string fromAirport;
	std::string toAirport;
	std::string departDate;
	std::string returnDate;
	std::vector<std::string> flightIDList;

	fromAirport = getUserInput(txn,
							   "From which airport? (please enter an airport code)",
							   "SELECT * FROM Airport ",
							   ("SELECT AirportCode, APName, City, Country "
									   "FROM Airport "
									   "WHERE AirportCode = "),
							   1);

	toAirport = getUserInput(txn,
							 "To which airport? (please enter an airport code)",
							 "SELECT * FROM Airport ",
							 ("SELECT AirportCode, APName, City, Country "
									 "FROM Airport "
									 "WHERE AirportCode = "),
							 1);

//		printf("Which airline would you like to book with?\n"); //might not need airline because customer might want
//		std::getline(std::cin, airline); //all airlines

	departDate = getDate("When will you be departing?");
//		printf("%s\n", departDate.c_str());
	returnDate = getDate("When will you be returning?");
//		printf("%s\n", returnDate.c_str());


	//get flight info
	getFlight(txn, "Please enter in a flight id (? to list all flights):",
			  flightIDList, fromAirport, toAirport, departDate, returnDate);
//	getFlight(txn, "Please enter in a flight id (? to list all flights):",
//			  flightIDList, "JFK", "YVR",
//			  "2015-01-01 00:00:00", "2020-12-31 00:00:00"); //for testing

	getGovIDs(txn, "Please enter your GovID", allPassengersGovID);

	return {std::move(allPassengersGovID), std::move(flightIDList)	};
}