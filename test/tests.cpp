#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <pqxx/pqxx>
#include <requests.hpp>
#include <handlers.hpp>
#include <prepared_statements.hpp>
#include "../include/dbConnectionParams.hpp"

struct _connection{
    pqxx::connection conn;
    _connection(): conn(DB_CONNECTION_STRING){
        jetdb::prepare(conn);
    }
    operator pqxx::connection&(){
        return conn;
    }
} connection;

//pqxx::connection connection = [](){
//    auto conn = pqxx::connection("host=localhost user=postgres");
//    jetdb::prepare(conn);
//    return conn;
//};
TEST_CASE("generic"){
  pqxx::work tx{connection};
  REQUIRE(
    jetdb::handlers::handle_request(
      tx,
      nlohmann::json{
        {"operation", "login"},
        {"name", "susan"},
        {"password", "password"}
      })
     == (nlohmann::json{
       {"success", true},
       {"reason", ""}
     }));
}

TEST_CASE("login"){
  pqxx::work tx{connection};
  REQUIRE(
    jetdb::handlers::handle_request(
      tx,
      jetdb::requests::login{"jenny", "password"})
     == jetdb::responses::result(true));
}

//TEST_CASE("login2 testing"){
//pqxx::work tx{connection};
//REQUIRE(
//        jetdb::handlers::handle_request(
//        tx,
//        jetdb::requests::login2{"test@test.com", "password"})
//        == jetdb::responses::result(true, "super secret"));
//REQUIRE(
//        jetdb::handlers::handle_request(
//        tx,
//        jetdb::requests::login2{"badtest@test.com", "password"})
//        == jetdb::responses::result(false, "not secret"));
//REQUIRE(
//        jetdb::handlers::handle_request(
//        tx,
//        jetdb::requests::login2{"test@test.com", "badpassword"})
//        == jetdb::responses::result(false, "not secret"));
//}

TEST_CASE("user authentication"){
	pqxx::work tx{connection};
    //use data from sampleDataJetDB.sql
	REQUIRE(	jetdb::handlers::handle_request(tx, jetdb::requests::login2{"test@test.com", "admin"}) ==
				jetdb::responses::result(true, "successful authentication"));
	REQUIRE(	jetdb::handlers::handle_request(tx, jetdb::requests::login2{"test@test.com", "badlogin"}) ==
				jetdb::responses::result(false, "invalid username/password"));
}