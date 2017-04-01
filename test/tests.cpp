#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <pqxx/pqxx>
#include <requests.hpp>
#include <handlers.hpp>
#include <json.hpp>
#include "../include/dbConnectionParams.hpp"

//struct _connection{
//    pqxx::connection conn;
//    _connection(): conn(DB_CONNECTION_STRING){
//        jetdb::prepare(conn);
//    }
//    operator pqxx::connection&(){
//        return conn;
//    }
//} connection;

//pqxx::connection connection = [](){
//    auto conn = pqxx::connection("host=localhost user=postgres");
//    jetdb::prepare(conn);
//    return conn;
//};

pqxx::connection connection(DB_CONNECTION_STRING);

TEST_CASE("generic"){
  pqxx::work tx{connection};

	auto jsonResponse = R"(
		  {
			  "success": true,
			  "reason": "",
			  "data":[null]
		  }
		)"_json;

  REQUIRE(
    jetdb::handlers::handle_request(
      tx,
      nlohmann::json{
        {"operation", "login"},
        {"name", "susan"},
        {"password", "password"}
      })
     == jsonResponse
	);
}

TEST_CASE("login"){
  pqxx::work tx{connection};
  REQUIRE(
    jetdb::handlers::handle_request(
      tx,
      jetdb::requests::login{"jenny", "password"})
     == jetdb::responses::result(true));
}


TEST_CASE("user authentication"){
	pqxx::work tx{connection};
    //use data from sampleDataJetDB.sql
	REQUIRE(	jetdb::handlers::handle_request(tx, jetdb::requests::login2{"test@test.com", "admin"}) ==
				jetdb::responses::result(true, "successful authentication", {}));
	REQUIRE(	jetdb::handlers::handle_request(tx, jetdb::requests::login2{"test@test.com", "badlogin"}) ==
				jetdb::responses::result(false, "invalid username/password"));
}

TEST_CASE("book flight")
{
	pqxx::work tx{connection};
	//use data from sampleDataJetDB.sql
	jetdb::responses::result goodRequest1 = jetdb::handlers::handle_request(tx, jetdb::requests::bookFlight{
			//clientGovID
			"12345678",
			{//otherPassengerGovIDs
					"22223333",
					"33334444"
			},
			{//flightIDList
					"1",
					"3",
					"4",
			}});

	jetdb::responses::result badRequest1 = jetdb::handlers::handle_request(tx, jetdb::requests::bookFlight{
			//clientGovID
			"not a govID",
			{//otherPassengerGovIDs
					"22223333",
					"33334444"
			},
			{//flightIDList
					"1",
					"3",
					"4",
			}});

//	std::cout << goodRequest1.success << goodRequest1.reason << goodRequest1.data << '\n';
//	std::cout << badRequest1.success << badRequest1.reason << badRequest1.data << '\n';

	REQUIRE(goodRequest1 == jetdb::handlers::bookFlight::successMsg);
	REQUIRE(badRequest1 == jetdb::handlers::bookFlight::failureMsg);
}

TEST_CASE("find passengers who flew with every airline (frequent fliers)")
{
	pqxx::work tx{connection};
	//use data from sampleDataJetDB.sql

	//try before adding data
	jetdb::responses::result response = jetdb::handlers::handle_request(tx, jetdb::requests::flewEveryAirline{});
	jetdb::responses::result expectedResult{true, "", nlohmann::json::parse(
			"{\"columns\":[\"govid\"],\"rows\":[]}")}; //TODO fix output format
	REQUIRE(response == expectedResult);

	jetdb::responses::result insertRequest = jetdb::handlers::handle_request(tx, jetdb::requests::bookFlight{
			//clientGovID
			"22223333",
			{//otherPassengerGovIDs
					"33334444"
			},
			{//flightIDList
					"1",
					"2",
					"3",
					"4",
					"5",
					"6",
					"7"
			}});

	//try after adding data
	response = jetdb::handlers::handle_request(tx, jetdb::requests::flewEveryAirline{});
	expectedResult = {true, "", nlohmann::json::parse("{\"columns\":[\"govid\"],\"rows\":[{\"govid\":\"22223333\"}, "
															  "{\"govid\":\"33334444\"}]}")}; //TODO fix output format

	REQUIRE(response == expectedResult);
}

TEST_CASE("add flight good case")
{
	pqxx::work tx{connection};
	int pID = 1;
	std::string prefix = "UAE";
	std::string fromAirportCode = "NRT";
	std::string toAirportCode = "CDG";
	std::string startTime = "2017-01-12 19:13:49.000000";
	std::string endTime = "2017-01-12 23:13:49.000000";
	int cost = 500;

	pqxx::result result = tx.exec("SELECT COUNT(*) FROM Flight;");
	int flightsCountInitial = result[0][0].as<int>();

	jetdb::responses::result goodRequest1 =
			jetdb::handlers::handle_request(
					tx, jetdb::requests::addFlight{
							pID, prefix, fromAirportCode, toAirportCode, startTime, endTime, cost
					}
			);

	result = tx.exec("SELECT COUNT(*) FROM Flight;");
	int flightsCountUpdated = result[0][0].as<int>();
//	std::cout << flightsCountInitial << " " << flightsCountUpdated << '\n';
	REQUIRE(flightsCountInitial+1 == flightsCountUpdated);

	REQUIRE(goodRequest1 == jetdb::handlers::addFlight::successMsg);
}

TEST_CASE("add flight bad case")
{
	pqxx::work tx{connection};
	int pID = 1;
	std::string prefix = "UAE";
	std::string fromAirportCode = "NRT";
	std::string toAirportCode = "CDG";
	std::string startTime = "2017-01-12 19:13:49.000000";
	std::string endTime = "2017-01-12 23:13:49.000000";
	int cost = 500;

	pqxx::result result = tx.exec("SELECT COUNT(*) FROM Flight;");
	int flightsCountInitial = result[0][0].as<int>();

	jetdb::responses::result badRequest1 =
			jetdb::handlers::handle_request(
					tx, jetdb::requests::addFlight{
							-1, "not a prefix", "not a fromAirportCode",
							toAirportCode, startTime, endTime, cost
					}
			);

	pqxx::work tx2{connection}; //TODO: unhack this later
	result = tx2.exec("SELECT COUNT(*) FROM Flight;");
	int flightsCountUpdated = result[0][0].as<int>();
//	std::cout << flightsCountInitial << " " << flightsCountUpdated << '\n';
	REQUIRE(flightsCountInitial == flightsCountUpdated); //shouldn't change with the failed update

	REQUIRE(badRequest1 == jetdb::handlers::addFlight::failureMsg);
}

TEST_CASE("add user good case")
{
	pqxx::work tx{connection};
	std::string email = "a@a.com";
	std::string password = "secret";
	int roleLevel = 1;

	pqxx::result result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
	int usersCountInitial = result[0][0].as<int>();

	jetdb::responses::result goodRequest1 =
			jetdb::handlers::handle_request(tx, jetdb::requests::addUser{ email, password, roleLevel });

	result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
	int usersCountUpdated = result[0][0].as<int>();
//	std::cout << usersCountInitial << " " << usersCountUpdated << '\n';
	REQUIRE(usersCountInitial+1 == usersCountUpdated);

	REQUIRE(goodRequest1 == jetdb::handlers::addUser::successMsg);
}


TEST_CASE("add user bad case")
{
	pqxx::work tx{connection};
	std::string email = "a@a.com";
	std::string password = "secret";
	int roleLevel = 4; //this should break it

	pqxx::result result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
	int usersCountInitial = result[0][0].as<int>();

	jetdb::responses::result badRequest1 =
			jetdb::handlers::handle_request(tx, jetdb::requests::addUser{ email, password, roleLevel });

	pqxx::work tx2{connection}; //TODO: unhack this later
	result = tx2.exec("SELECT COUNT(*) FROM LoginUser;");
	int usersCountUpdated = result[0][0].as<int>();
//	std::cout << usersCountInitial << " " << usersCountUpdated << '\n';
	REQUIRE(usersCountInitial == usersCountUpdated);

	REQUIRE(badRequest1 == jetdb::handlers::addUser::failureMsg);
}
