#pragma once

#include <exception>
#include <stdexcept>
#include <json.hpp>
#include <pqxx/pqxx>

// add handlers here
#include <handlers/login.hpp>
#include <handlers/login2.hpp>

#include <handlers/available_flights.hpp>
#include <handlers/get_airports.hpp>
#include <handlers/get_airlines.hpp>

namespace jetdb{
  namespace handlers{
    template<typename Request>
    auto request_handler(){
      return [](pqxx::work& txn, nlohmann::json js) -> nlohmann::json{
        return handle_request(txn, static_cast<Request>(std::move(js)));
      };
    }

    std::map<std::string, nlohmann::json(*)(pqxx::work&, nlohmann::json)> const _request_handlers{
      {"login", request_handler<requests::login>()},
      {"login2", request_handler<requests::login2>()},
      {"available_flights", request_handler<requests::available_flights>()},
      {"get_airports", request_handler<requests::get_airports>()},
      {"get_airlines", request_handler<requests::get_airlines>()}
    };

    nlohmann::json handle_request(pqxx::work& txn, nlohmann::json req){
      return _request_handlers.at(req["operation"])(txn, std::move(req));
    }
  }
}
