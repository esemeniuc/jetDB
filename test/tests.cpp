#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <pqxx/pqxx>
#include <requests.hpp>
#include <handlers.hpp>

pqxx::connection connection("host=localhost user=postgres");

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

TEST_CASE("login2 testing"){
pqxx::work tx{connection};
REQUIRE(
        jetdb::handlers::handle_request(
        tx,
        jetdb::requests::login2{"test@test.com", "password"})
        == jetdb::responses::result(true, "super secret"));
REQUIRE(
        jetdb::handlers::handle_request(
        tx,
        jetdb::requests::login2{"badtest@test.com", "password"})
        == jetdb::responses::result(false, "not secret"));
REQUIRE(
        jetdb::handlers::handle_request(
        tx,
        jetdb::requests::login2{"test@test.com", "badpassword"})
        == jetdb::responses::result(false, "not secret"));
}