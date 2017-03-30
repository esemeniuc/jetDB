#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::get_airlines req){
      pqxx::result result;
      std::string prefix = req.prefix;
      std::string alname = req.alname;
      txn.conn().prepare(
        "airlines",
        "SELECT * FROM Airline WHERE "
        "  ($1 = '' OR prefix = $1)"
        "  AND ($2 = '' OR alname = $2)"
      );
      result = txn.prepared("airlines")(prefix)(alname).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}



