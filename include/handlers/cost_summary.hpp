#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::cost_summary req){
      pqxx::result result;
      std::string from_airportcode = req.from_airportcode;
      std::string to_airportcode = req.to_airportcode;

      std::string query =
        "SELECT min(cost), max(cost), avg(cost), count(*) "
        "FROM flight "
        "WHERE ($1 = '' OR fromairportcode = $1)"
        " AND ($2 = '' OR toairportcode = $2)";
      txn.conn().prepare("cost_summary", query);
      result = txn.prepared("cost_summary")(from_airportcode)(to_airportcode).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}

