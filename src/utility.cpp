//
// Created by eric on 3/29/17.
//
#include "utility.h"

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
