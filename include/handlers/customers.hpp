#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::customers req){
      pqxx::result result;
      std::string govid = req.govid;
      txn.conn().prepare(
        "customers",
        "SELECT * FROM client WHERE "
        "  ($1 = '' OR govid = $1)"
      );
      result = txn.prepared("customers")(govid).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}
