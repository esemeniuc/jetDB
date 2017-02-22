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


int getGovID(pqxx::work& txn)
{
	std::string govID;

	std::string lookupString = ("SELECT GovID, CName FROM Client");

	std::string validateString = ("SELECT GovID, CName "
								  "FROM Client "
								  "WHERE GovID =" + txn.quote(govID));

	bool validInput = false;
	while(validInput == false)
	{
		printf("Please enter your GovID\n");

		std::getline(std::cin, govID);

		if(govID == "?")
		{
			lookupPrint(lookupString, txn);
		}
		else if(!isValid(validateString, txn))
		{
			printf("Invalid entry, please try again\n");
		}
		else
		{
			validInput = true;
			txn.exec(validateString);
			printf("%s\n", row[0].c_str());

		}
	}

	return -1;
}

bool validatePrint(std::string validateString, pqxx::work& txn)
{
	pqxx::result validateResult = txn.exec(validateString);
	if (validateResult.size() == 0)
	{
		printf("No results found\n");
	}
	else
	{
		printf("Found: %lu results\n", validateResult.size());
		int n = validateResult.columns();
//		printf("%d columns\n", validateResult.columns());
		for(int i = 0; i < n; i++) //print columns
		{
			if(i != n-1)
			{
				printf("%s\t", validateResult.column_name(i));
			}
			else
			{
				printf("%s\n", validateResult.column_name(i));
			}
		}

		for (auto row: validateResult) //print results
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

	if(validateResult.size() == 1)
	{
		return true;
	}
	return false;
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

int getInfo(pqxx::work& txn)
{
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
	while(true)
	{
		printf("Booking a flight\n");
		printf("Enter '?' to lookup values\n");



		printf("Which airline would you like to book with?\n");
		std::getline(std::cin, airline);

		printf("From which airport?\n");
		std::getline(std::cin, fromAirport);

//		printf("To which airport?\n"); //might not need airline because customer might want
//		std::getline(std::cin, airline); //all airlines

		printf("When will you be departing?\n");
		std::getline(std::cin, departDate);

		printf("When will you be returning?\n");
		std::getline(std::cin, returnDate);

		lookupString = ("SELECT fid"
						"FROM flight "
						"JOIN "
						"WHERE Model =" + txn.quote(departDate));

//		SELECT * FROM flight
//		NATURAL JOIN airline --need prefix for flightname
//		NATURAL JOIN flightname --needed for named
//		NATURAL JOIN named

		printf("Please enter in a flight from the selections below:?\n");
//		for (auto row: lookupResult)
//		{
//			printf("%s\n", row[0].c_str());
//		}
		std::getline(std::cin, flightNum);

		printf("Thank you\n\n");

		printf("Book another flight?\n");
		std::getline(std::cin, loopStatus);
		if(loopStatus == "n")
		{
			break;
		}

	}
	return 5;
}