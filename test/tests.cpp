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

TEST_CASE("book flight"){
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