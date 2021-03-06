#pragma once

#include <exception>
#include <stdexcept>
#include <json.hpp>
#include <pqxx/pqxx>

// add handlers here
#include <handlers/login.hpp>
#include <handlers/login2.hpp>
#include <handlers/addUser.hpp>
#include <handlers/addFlight.hpp>
#include <handlers/bookFlight.hpp>
#include <handlers/deleteBooking.hpp>
#include <handlers/flewEveryAirline.hpp>
#include <handlers/updateRingLevel.hpp>

#include <handlers/available_flights.hpp>
#include <handlers/get_airports.hpp>
#include <handlers/get_airlines.hpp>
#include <handlers/avg_costs.hpp>
#include <handlers/find_paths.hpp>
#include <handlers/cost_summary.hpp>
#include <handlers/customers.hpp>
#include <handlers/get_bookings.hpp>

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
      {"addUser", request_handler<requests::addUser>()},
      {"addFlight", request_handler<requests::addFlight>()},
      {"bookFlight", request_handler<requests::bookFlight>()},
      {"deleteBooking", request_handler<requests::deleteBooking>()},
      {"flewEveryAirline", request_handler<requests::flewEveryAirline>()},
      {"updateRingLevel", request_handler<requests::updateRingLevel>()},
      {"available_flights", request_handler<requests::available_flights>()},
      {"get_airports", request_handler<requests::get_airports>()},
      {"get_airlines", request_handler<requests::get_airlines>()},
      {"avg_costs", request_handler<requests::avg_costs>()},
      {"find_paths", request_handler<requests::find_paths>()},
      {"cost_summary", request_handler<requests::cost_summary>()},
      {"customers", request_handler<requests::customers>()},
      {"get_bookings", request_handler<requests::get_bookings>()},
    };

    nlohmann::json handle_request(pqxx::work& txn, nlohmann::json req){
      return _request_handlers.at(req["operation"])(txn, std::move(req));
    }
  }
}
