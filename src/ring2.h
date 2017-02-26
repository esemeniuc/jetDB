//
// Created by eric on 2/18/17.
//
int bookFlightByID(pqxx::connection_base& c, std::tuple<std::vector<std::string>, std::vector<std::string>> userInfo);

bool isValid(std::string validateString, pqxx::work& txn);

void printResult(const pqxx::result& printResult);

pqxx::result::size_type lookupPrint(std::string lookupString, pqxx::work& txn);

std::string getUserInput(pqxx::work& txn,
						 std::string promptUserString,
						 std::string lookupString,
						 std::string validationString,
						 int numParamsForValidation);

std::string getDate(std::string promptUserString);

int getFlight(pqxx::work& txn,
			  std::string promptUserString,
			  std::vector<std::string>& flightIDList,
			  const std::string& fromAirport,
			  const std::string& toAirport,
			  const std::string& departDate,
			  const std::string& returnDate);

int getGovIDs(pqxx::work& txn, std::string promptUserString, std::vector<std::string>& allPassengersGovID);

std::tuple<std::vector<std::string>, std::vector<std::string>> getBookingInfo(pqxx::connection_base& c);