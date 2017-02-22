//
// Created by eric on 2/18/17.
//
int bookFlightByID(std::string GovID, int DateBooked, std::vector<int> flightID);

bool isValid(std::string validateString, pqxx::work& txn);

pqxx::result::size_type lookupPrint(std::string lookupString, pqxx::work& txn);

void printResult(pqxx::result& printResult);

std::string getUserInput(pqxx::work& txn,
						 std::string promptUserString,
						 std::string lookupString,
						 std::string validationString,
						 int numParamsForValidation);

int getInfo(pqxx::work& txn);
