//
// Created by eric on 2/18/17.
//
#include <cstdio>
#include <iostream>
#include <vector>
#include <pqxx/pqxx>
#include "ring2.h"
#include <ctime>

//preconditions: GovIDs and flight IDs in the db
//description: use to book a flight after acquiring all valid user info from interactive prompt or GUI
//postconditions: returns 0 on successful booking, nonzero otherwise
int bookFlightByID(pqxx::connection_base& c, std::tuple<std::vector<std::string>, std::vector<std::string>> userInfo)
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
	int temp = 0;

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

	return std::tie(allPassengersGovID, flightIDList);
}