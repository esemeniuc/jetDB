#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::available_flights req){
      pqxx::result result;
      std::string prefix = req.prefix;
      std::string airportcode = req.airportcode;
      txn.conn().prepare(
        "available_flights",
        "SELECT * FROM Flight WHERE "
        "  ($1 = '' OR prefix = $1)"
        "  AND ($2 = '' OR fromairportcode = $2 OR toairportcode = $2)"
      );
      result = txn.prepared("available_flights")(prefix)(airportcode).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}

