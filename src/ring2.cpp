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
int bookFlightByID(std::string GovID, int DateBooked, std::vector<int> flightID)
{

	return 1;
}

bool isValid(std::string validateString, pqxx::work& txn)
{
	return (txn.exec(validateString).size() > 0);
}

//prints out result
void printResult(pqxx::result& printResult)
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
		for(int i = 0; i < n; i++) //print columns
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
		for(int i = 0; i < n; i++) //print columns
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


std::string getUserInput(pqxx::work& txn,
						 std::string promptUserString,
						 std::string lookupString,
						 std::string validationString,
						 int numParamsForValidation)
{
	std::string userInput;
	pqxx::result validationResult;

	bool validInput = false;
	while(validInput == false)
	{
		printf("%s\n", promptUserString.c_str());

		std::getline(std::cin, userInput);

		if(userInput == "?")
		{
			lookupPrint(lookupString, txn);
			continue; //don't run the rest
		}

		validationResult = txn.exec(validationString + txn.quote(userInput));
		printf("%lu rows\n", validationResult.size());
		if(validationResult.size() != 1)
		{
			printf("Invalid entry, please try again\n");
		}
		else //assume valid
		{
			validInput = true;
			printResult(validationResult);
		}
	}

	return userInput;
}


int getInfo(pqxx::work& txn)
{
	int govID;
	std::string airline;
	std::string fromAirport;
	std::string toAirport;
	std::string departDate;
	std::string returnDate;
	std::string flightNum;
	std::string loopStatus;
	std::vector<std::string> otherPassengersGovID;
	std::string lookupString;
	std::string validateString;
	while(loopStatus != "n")
	{
		printf("Booking a flight\n");
		printf("Enter '?' to lookup values\n");

//		govID = getGovID(txn);
		std::string govID = getUserInput(txn,
										 "Please enter your GovID",
										 "SELECT GovID, CName FROM Client",
										 ("SELECT GovID, CName "
												 "FROM Client "
												 "WHERE GovID = "),
										 1);

		std::string fromAirport = getUserInput(txn,
											   "From which airport? (please enter an airport code)",
											   "SELECT * FROM Airport ",
											   ("SELECT AirportCode, APName, City, Country "
													   "FROM Airport "
													   "WHERE AirportCode = "),
											   1);

		std::string toAirport = getUserInput(txn,
											 "To which airport? (please enter an airport code)",
											 "SELECT * FROM Airport ",
											 ("SELECT AirportCode, APName, City, Country "
													 "FROM Airport "
													 "WHERE AirportCode = "),
											 1);

//		printf("Which airline would you like to book with?\n"); //might not need airline because customer might want
//		std::getline(std::cin, airline); //all airlines

		printf("When will you be departing?\n");
		std::getline(std::cin, departDate);

		printf("When will you be returning?\n");
		std::getline(std::cin, returnDate);



		printf("Please enter in a flight from the selections below:?\n");
//		for (auto row: lookupResult)
//		{
//			printf("%s\n", row[0].c_str());
//		}
		std::getline(std::cin, flightNum);


		printf("Thank you\n\nBook another flight?\n");
		std::getline(std::cin, loopStatus);
	}
	return 5;
}