#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::get_airports req){
      pqxx::result result;
      std::string airportcode = req.airportcode;
      std::string apname = req.apname;
      txn.conn().prepare(
        "airports",
        "SELECT * FROM Airport WHERE "
        "  ($1 = '' OR airportcode = $1)"
        "  AND ($2 = '' OR apname = $2)"
      );
      result = txn.prepared("airports")(airportcode)(apname).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}


