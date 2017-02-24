//
// Created by eric on 2/18/17.
//
int bookFlightByID(pqxx::work& txn, std::tuple<std::string, std::string, std::vector<std::string>>);

bool isValid(std::string validateString, pqxx::work& txn);

void printResult(const pqxx::result& printResult);

pqxx::result::size_type lookupPrint(std::string lookupString, pqxx::work& txn);

std::string getUserInput(pqxx::work& txn,
						 std::string promptUserString,
						 std::string lookupString,
						 std::string validationString,
						 int numParamsForValidation);

std::string getDate(std::string promptUserString);

std::string getFlight(pqxx::work& txn,
											  std::string promptUserString,
											  const std::string& fromAirport,
											  const std::string& toAirport,
											  const std::string& departDate,
											  const std::string& returnDate);

std::tuple<std::string, std::string, std::vector<std::string>> getBookingInfo(pqxx::work& txn);