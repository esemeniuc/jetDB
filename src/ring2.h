//
// Created by eric on 2/18/17.
//
int bookFlightByID(std::string GovID, int DateBooked, std::vector<int> flightID);

int getInfo(pqxx::work& txn);

bool isValid(std::string validateString, pqxx::work& txn);

pqxx::result::size_type lookupPrint(std::string lookupString, pqxx::work& txn);
bool validatePrint(std::string validateString, pqxx::work& txn);
int getGovID(pqxx::work& txn);