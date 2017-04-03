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
			  "data":null
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
	//use data from sampleDataJetDB.sql
  {
    pqxx::work tx{connection};
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
    REQUIRE(goodRequest1 == jetdb::handlers::bookFlight::successMsg);
  }

  {
    pqxx::work tx{connection};
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
    REQUIRE(badRequest1 == jetdb::handlers::bookFlight::failureMsg);
  }

//	std::cout << goodRequest1.success << goodRequest1.reason << goodRequest1.data << '\n';
//	std::cout << badRequest1.success << badRequest1.reason << badRequest1.data << '\n';
}

TEST_CASE("find passengers who flew with every airline (frequent fliers)")
{
	//use data from sampleDataJetDB.sql

	//try before adding data
  {
    pqxx::work tx{connection};
  	jetdb::responses::result response = jetdb::handlers::handle_request(tx, jetdb::requests::flewEveryAirline{});
  	jetdb::responses::result expectedResult{true, "", nlohmann::json::parse(
  			"{\"columns\":[\"govid\"],\"rows\":[]}")}; //TODO fix output format
  	REQUIRE(response == expectedResult);
  }

  {
    pqxx::work tx{connection};
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
  }

  {
    pqxx::work tx{connection};
  	//try after adding data
  	jetdb::responses::result response = jetdb::handlers::handle_request(tx, jetdb::requests::flewEveryAirline{});
  	jetdb::responses::result expectedResult = {true, "", nlohmann::json::parse("{\"columns\":[\"govid\"],\"rows\":[{\"govid\":\"22223333\"}, "
  															  "{\"govid\":\"33334444\"}]}")}; //TODO fix output format
    REQUIRE(response == expectedResult);
  }
}

TEST_CASE("add flight good case")
{
	int pID = 1;
	std::string prefix = "UAE";
	std::string fromAirportCode = "NRT";
	std::string toAirportCode = "CDG";
	std::string startTime = "2017-01-12 19:13:49.000000";
	std::string endTime = "2017-01-12 23:13:49.000000";
	int cost = 500;
  int flightsCountInitial;
	{
    pqxx::work tx{connection};
    pqxx::result result = tx.exec("SELECT COUNT(*) FROM Flight;");
	  flightsCountInitial = result[0][0].as<int>();
  }

  jetdb::responses::result goodRequest1;
  {
    pqxx::work tx{connection};
  	goodRequest1 =
  			jetdb::handlers::handle_request(
  					tx, jetdb::requests::addFlight{
  							pID, prefix, fromAirportCode, toAirportCode, startTime, endTime, cost
  					}
  			);
  }

  int flightsCountUpdated;
	{
    pqxx::work tx{connection};
    pqxx::result result = tx.exec("SELECT COUNT(*) FROM Flight;");
	  flightsCountUpdated = result[0][0].as<int>();
  }
//	std::cout << flightsCountInitial << " " << flightsCountUpdated << '\n';
  REQUIRE(goodRequest1 == jetdb::handlers::addFlight::successMsg);
	REQUIRE(flightsCountInitial+1 == flightsCountUpdated);
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

	pqxx::work tx2{connection};
	result = tx2.exec("SELECT COUNT(*) FROM Flight;");
	int flightsCountUpdated = result[0][0].as<int>();
//	std::cout << flightsCountInitial << " " << flightsCountUpdated << '\n';
	REQUIRE(flightsCountInitial == flightsCountUpdated); //shouldn't change with the failed update

	REQUIRE(badRequest1 == jetdb::handlers::addFlight::failureMsg);
}

TEST_CASE("add user good case")
{
	std::string email = "a@a.com";
	std::string password = "secret";
	int roleLevel = 1;

  int usersCountInitial;
  {
    pqxx::work tx{connection};
  	pqxx::result result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
  	usersCountInitial = result[0][0].as<int>();
  }

  {
    pqxx::work tx{connection};
  	jetdb::responses::result goodRequest1 =
  			jetdb::handlers::handle_request(tx, jetdb::requests::addUser{ email, password, roleLevel });
    REQUIRE(goodRequest1 == jetdb::handlers::addUser::successMsg);
  }

  {
    pqxx::work tx{connection};
    pqxx::result result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
  	int usersCountUpdated = result[0][0].as<int>();
  	REQUIRE(usersCountInitial+1 == usersCountUpdated);
  }
}


TEST_CASE("add user bad case")
{
	std::string email = "a@a.com";
	std::string password = "secret";
	int roleLevel = 7; //this should break because of the check constraint being valid from 0 to 3

  int usersCountInitial;
  {
    pqxx::work tx{connection};
    pqxx::result result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
  	usersCountInitial = result[0][0].as<int>();
  }

	{
    pqxx::work tx{connection};
    pqxx::result result = tx.exec("INSERT INTO Role (RingLevel, RingDesc) VALUES ('7','SUPA HACKA');"); //avoid fkey check
  }

	{
    pqxx::work tx{connection};
    jetdb::responses::result badRequest1 =
			jetdb::handlers::handle_request(tx, jetdb::requests::addUser{ email, password, roleLevel });
    REQUIRE(badRequest1 == jetdb::handlers::addUser::failureMsg);
  }

  {
    pqxx::work tx{connection};
  	pqxx::result result = tx.exec("SELECT COUNT(*) FROM LoginUser;");
  	int usersCountUpdated = result[0][0].as<int>();
  	REQUIRE(usersCountInitial == usersCountUpdated);
  }
}

TEST_CASE("delete booking good case")
{
	int bid = 1;

  int bookingsCountInitial;
  {
    pqxx::work tx{connection};
  	pqxx::result result = tx.exec("SELECT COUNT(*) FROM Booking;");
  	bookingsCountInitial = result[0][0].as<int>();
  }

  {
    pqxx::work tx{connection};
    jetdb::responses::result goodRequest1 =
  			jetdb::handlers::handle_request(tx, jetdb::requests::deleteBooking{ bid });
    REQUIRE(goodRequest1 == jetdb::handlers::deleteBooking::successMsg);
  }

  {
    pqxx::work tx{connection};
    pqxx::result result = tx.exec("SELECT COUNT(*) FROM Booking;");
  	int bookingsCountUpdated = result[0][0].as<int>();
  	REQUIRE(bookingsCountInitial == bookingsCountUpdated+1);
  }
}

TEST_CASE("delete booking bad case")
{
	int bid = 999999999;

  int bookingsCountInitial;
  {
    pqxx::work tx{connection};
  	pqxx::result result = tx.exec("SELECT COUNT(*) FROM Booking;");
  	bookingsCountInitial = result[0][0].as<int>();
  }

  {
    pqxx::work tx{connection};
    jetdb::responses::result badRequest1 =
  			jetdb::handlers::handle_request(tx, jetdb::requests::deleteBooking{ bid });
    REQUIRE(badRequest1 == jetdb::handlers::deleteBooking::failureMsg);
  }

  {
    pqxx::work tx{connection};
  	pqxx::result result = tx.exec("SELECT COUNT(*) FROM Booking;");
  	int bookingsCountUpdated = result[0][0].as<int>();
  	REQUIRE(bookingsCountInitial == bookingsCountUpdated);
  }

}

TEST_CASE("update ring level good case")
{
	pqxx::work tx{connection};
	std::string email = "test@test.com";
	int ringLevel = 1;

	jetdb::responses::result goodRequest1 =
			jetdb::handlers::handle_request(tx, jetdb::requests::updateRingLevel{ email, ringLevel });

//	std::cout << usersCountInitial << " " << usersCountUpdated << '\n';
	REQUIRE(goodRequest1 == jetdb::handlers::updateRingLevel::successMsg);
}


TEST_CASE("update ring level bad case")
{
	pqxx::work tx{connection};
	std::string email = "test@test.com";
	int ringLevel = 7; //this should break because of the check constraint being valid from 0 to 3
	pqxx::result result = result = tx.exec("INSERT INTO Role (RingLevel, RingDesc) VALUES ('7','SUPA HACKA');"); //avoid fkey check

	tx.conn().prepare("checkRingLevel", "SELECT RingLevel FROM LoginUser WHERE Email = $1;"); //check our current level
	result = tx.prepared("checkRingLevel")(email).exec();
	int initialRingLevel = result[0][0].as<int>();

	jetdb::responses::result badRequest1 =
			jetdb::handlers::handle_request(tx, jetdb::requests::updateRingLevel{ email, ringLevel });

	pqxx::work tx2{connection}; //TODO: unhack this later
	tx2.conn().prepare("checkRingLevel", "SELECT RingLevel FROM LoginUser WHERE Email = $1;");
	result = tx2.prepared("checkRingLevel")(email).exec();
	int currentRingLevel = result[0][0].as<int>();
//	std::cout << usersCountInitial << " " << usersCountUpdated << '\n';
	REQUIRE(initialRingLevel == currentRingLevel);
	REQUIRE(badRequest1 == jetdb::handlers::updateRingLevel::failureMsg);
}