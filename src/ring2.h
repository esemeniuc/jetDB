//
// Created by eric on 2/18/17.
//
#include <vector>
#include <pqxx/pqxx>

int bookFlightByIDTuple(pqxx::connection_base& c, std::tuple<std::vector<std::string>, std::vector<std::string>> userInfo);
int bookFlightByID(pqxx::connection_base& c, std::string clientGovID, std::vector<std::string> otherPassengerGovIDs,
				   std::vector<std::string> flightIDList);

int getFlight(pqxx::work& txn,
			  std::string promptUserString,
			  std::vector<std::string>& flightIDList,
			  const std::string& fromAirport,
			  const std::string& toAirport,
			  const std::string& departDate,
			  const std::string& returnDate);

int getGovIDs(pqxx::work& txn, std::string promptUserString, std::vector<std::string>& allPassengersGovID);

std::tuple<std::vector<std::string>, std::vector<std::string>> getBookingInfo(pqxx::connection_base& c);