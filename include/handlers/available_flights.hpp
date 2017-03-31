#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::available_flights req){
      pqxx::result result;
      std::string prefix = req.prefix;
      std::string airportcode = req.airportcode;
      int fid = req.fid;
      txn.conn().prepare(
        "available_flights",
        "SELECT * FROM Flight WHERE "
        "  ($1 = '' OR prefix = $1)"
        "  AND ($2 = '' OR fromairportcode = $2 OR toairportcode = $2)"
        "  AND ($3 = -1 OR fid = $3)"
      );
      result = txn.prepared("available_flights")(prefix)(airportcode)(fid).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}

