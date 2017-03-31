#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::avg_costs req){
      pqxx::result result;
      std::string kind = req.kind;

      std::string select_query = "SELECT prefix, AVG(cost) FROM flight GROUP BY prefix",
        base_query_pre =
          "WITH t AS (" + select_query + ")"
          "  SELECT t.prefix, t.avg FROM t WHERE t.avg = (SELECT ",
        base_query_post = /* MIN/MAX */ "(t.avg) FROM t)";

      txn.conn().prepare("avg_costs", select_query);
      txn.conn().prepare("min_avg_costs", base_query_pre + "MIN" + base_query_post);
      txn.conn().prepare("max_avg_costs", base_query_pre + "MAX" + base_query_post);

      std::string query_name;
      if (kind == "") query_name = "avg_costs";
      else if (kind == "min") query_name = "min_avg_costs";
      else if (kind == "max") query_name = "max_avg_costs";
      else return {false, "Invalid kind: '" + kind + "'.", {}};

      result = txn.prepared(query_name).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}



