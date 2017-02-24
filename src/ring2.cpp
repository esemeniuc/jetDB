//
// Created by eric on 2/18/17.
//
#include <cstdio>
#include <iostream>
#include <vector>
#include <pqxx/pqxx>
#include "ring2.h"


//preconditions: GovID in db
//description: use to book a flight after acquiring all valid user info from interactive prompt or GUI
//postconditions: returns 0 on successful booking, nonzero otherwise
int bookFlightByID(pqxx::work& txn, std::tuple<std::string, std::string, std::vector<std::string>>)
{
	//unpack data
	//get<0> = govID
	//get<1> = flightID
	//get<2> = otherPassengerGovIDs


	//insert into db
	std::string bookingInsert = "INSERT INTO booking (bid) VALUES (DEFAULT) RETURNING bid";

	pqxx::result = txn.exec(bookingInsert);

	std::string booksInsert = "INSERT INTO books (bid) VALUES (DEFAULT) RETURNING bid";

	return 1;
}

bool isValid(std::string validateString, pqxx::work& txn)
{
	return (txn.exec(validateString).size() > 0);
}

//prints out result of a query
void printResult(const pqxx::result& printResult)
{
	if (printResult.size() == 0)
	{
		printf("No results found\n");
	}
	else
	{
		printf("Found: %lu results\n", printResult.size());
		int n = printResult.columns();
//		printf("%d columns\n", printResult.columns());
		for(pqxx::tuple_size_type i = 0; i < n; i++) //print columns
		{
			if(i != n-1)
			{
				printf("%s\t", printResult.column_name(i));
			}
			else
			{
				printf("%s\n", printResult.column_name(i));
			}
		}

		for (auto row: printResult) //print results
		{
			for(int i = 0; i < n; i++)
			{
				if(i != n-1)
				{
					printf("%s\t", row[i].c_str());
				}
				else
				{
					printf("%s\n", row[i].c_str());
				}
			}
		}
	}
}

//prints out the contents and returns the # of rows
pqxx::result::size_type lookupPrint(std::string lookupString, pqxx::work& txn)
{
	pqxx::result lookupResult = txn.exec(lookupString);
	if (lookupResult.size() == 0)
	{
		printf("No results found\n");
	}
	else
	{
		printf("Found: %lu results\n", lookupResult.size());
		int n = lookupResult.columns();
//		printf("%d columns\n", lookupResult.columns());
		for(pqxx::tuple_size_type i = 0; i < n; i++) //print columns
		{
			if(i != n-1)
			{
				printf("%s\t", lookupResult.column_name(i));
			}
			else
			{
				printf("%s\n", lookupResult.column_name(i));
			}
		}

		for (auto row: lookupResult) //print results
		{
			for(int i = 0; i < n; i++)
			{
				if(i != n-1)
				{
					printf("%s\t", row[i].c_str());
				}
				else
				{
					printf("%s\n", row[i].c_str());
				}
			}
		}
	}
	return lookupResult.size();
}

//generic function for getting a field
std::string getUserInput(pqxx::work& txn,
						 std::string promptUserString,
						 std::string lookupString,
						 std::string validationString,
						 int numParamsForValidation)
{
	std::string userInput;
	pqxx::result validationResult;

	bool validInput = false;
	do
	{
		printf("%s\n", promptUserString.c_str());

		std::getline(std::cin, userInput);

		if(userInput == "?")
		{
			lookupPrint(lookupString, txn);
			continue; //don't run the rest
		}

		validationResult = txn.exec(validationString + txn.quote(userInput));
		if(validationResult.size() != 1)
		{
			printf("Invalid entry, please try again\n");
		}
		else //assume valid
		{
			validInput = true;
			printResult(validationResult);
		}
	} while(validInput == false);

	return userInput;
}


//get date in 2016-10-16 23:39:37.000000 format
std::string getDate(std::string promptUserString)
{
	std::string timeString;
	std::string userInput;
	int temp;
	bool validInput = false;

	printf("%s\n", promptUserString.c_str());

	do //get year
	{
		printf("Please enter in a year (2015 to 2020)\n");
		std::getline(std::cin, userInput);

		try
		{
			temp = std::stoi(userInput);
		} catch (const std::invalid_argument& ia)
		{
			printf("Non numeric input received\n");
			continue;
		}
	} while(temp < 2015 || temp > 2020);
	timeString += std::to_string(temp) + "-";
	temp = -1; //reset temp

	do //get month
	{
		printf("Please enter in a month (1 to 12)\n");
		std::getline(std::cin, userInput);

		try
		{
			temp = std::stoi(userInput);
		} catch (const std::invalid_argument& ia)
		{
			printf("Non numeric input received\n");
			continue;
		}
	} while(temp < 1 || temp > 12);
	timeString += std::to_string(temp) + "-";

	do //get day
	{
		printf("Please enter in a day (1 to 31)\n");
		std::getline(std::cin, userInput);

		try
		{
			temp = std::stoi(userInput);
		} catch (const std::invalid_argument& ia)
		{
			printf("Non numeric input received\n");
			continue;
		}
	} while(temp < 1 || temp > 31);
	timeString += std::to_string(temp) + " ";

	do //get hour
	{
		printf("Please enter in an hour (0 to 23)\n");
		std::getline(std::cin, userInput);

		try
		{
			temp = std::stoi(userInput);
		} catch (const std::invalid_argument& ia)
		{
			printf("Non numeric input received\n");
			continue;
		}
	}while(temp < 0 || temp > 23);
	timeString += std::to_string(temp) + ":";

	do //get minute
	{
		printf("Please enter in a minute (0 to 59)\n");
		std::getline(std::cin, userInput);

		try
		{
			temp = std::stoi(userInput);
		} catch (const std::invalid_argument& ia)
		{
			printf("Non numeric input received\n");
			continue;
		}
	} while(temp < 0 || temp > 59);
	timeString += std::to_string(temp) + ":00"; //don't worry about seconds

	return timeString;
}

//make query based on all the where conditions
//from, to airport code, based on the 2 date ranges
//returns the flight no
std::string getFlight(pqxx::work& txn,
			  std::string promptUserString,
			  const std::string& fromAirport,
			  const std::string& toAirport,
			  const std::string& departDate,
			  const std::string& returnDate)
{
	bool validInput;
	std::string userInput;
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

	do //list all flights
	{
		printf("%s\n", promptUserString.c_str());
		std::getline(std::cin, userInput); //expecting in put for a flight id

		if(userInput == "?")
		{
			lookupPrint(lookupString, txn);
			continue; //don't run the rest
		}

		pqxx::result validationResult = txn.exec(validationString + txn.quote(userInput));
		if(validationResult.size() != 1)
		{
			printf("Invalid entry, please try again\n");
		}
		else //assume valid
		{
			validInput = true;
			printResult(validationResult);
		}
	} while(validInput == false);

	return userInput;
}

std::tuple<std::string, std::string, std::vector<std::string>> getBookingInfo(pqxx::work& txn)
{
	std::string govID;
	std::string fromAirport;
	std::string toAirport;
	std::string departDate;
	std::string returnDate;
	std::string flightID;
	std::string loopStatus;
	std::vector<std::string> otherPassengersGovID;
	do
	{
		printf("Booking a flight\n");
		printf("Enter '?' to lookup acceptable input\n");

//		fromAirport = getUserInput(txn,
//								   "From which airport? (please enter an airport code)",
//								   "SELECT * FROM Airport ",
//								   ("SELECT AirportCode, APName, City, Country "
//										   "FROM Airport "
//										   "WHERE AirportCode = "),
//								   1);
//
//		toAirport = getUserInput(txn,
//								 "To which airport? (please enter an airport code)",
//								 "SELECT * FROM Airport ",
//								 ("SELECT AirportCode, APName, City, Country "
//										 "FROM Airport "
//										 "WHERE AirportCode = "),
//								 1);
//
////		printf("Which airline would you like to book with?\n"); //might not need airline because customer might want
////		std::getline(std::cin, airline); //all airlines
//
//		departDate = getDate("When will you be departing");
////		printf("%s\n", departDate.c_str());
//		returnDate = getDate("When will you be returning");
////		printf("%s\n", returnDate.c_str());
//
//
//		//get flight info
//		int flightCode =
//				getFlight(txn, "Please enter in a flight id (? to list all flights):",
//						  fromAirport, toAirport,
//						  departDate, returnDate);
		flightID =
				getFlight(txn, "Please enter in a flight id (? to list all flights):",
						  "JFK", "YVR",
						  "2015-01-01 00:00:00", "2020-12-31 00:00:00"); //for testing

		govID = getUserInput(txn,
							 "Please enter your GovID",
							 "SELECT GovID, CName FROM Client",
							 ("SELECT GovID, CName "
									 "FROM Client "
									 "WHERE GovID = "),
							 1);

		printf("Thank you\n\nBook another flight?\n");
		std::getline(std::cin, loopStatus);
	} while(loopStatus != "n");

	return std::tie(govID, flightID, otherPassengersGovID); //get booking holder govID, flightID, and other
	// passengers govID
}