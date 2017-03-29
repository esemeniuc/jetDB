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

#if 0
      std::stringstream ss;
      int n = result.size();
      int m = result.columns();
      ss << "Number of results: " << n << std::endl;
      ss << std::endl;
      std::stringstream tt;
      for(pqxx::tuple_size_type i = 0; i < m; i++)
      {
        ss << std::setw(20) << result.column_name(i);
        for (int j = 0; j < 20; j++)
          tt << "-";

        if (i + 1 < m) {
          ss << " | ";
          tt << "---";
        }
      }

      ss << std::endl;
      ss << tt.str() << std::endl;
      for (auto row : result) {
        for (pqxx::tuple_size_type i = 0; i < m; i++) {
          ss << std::setw(20) << row[i].c_str();
          if (i + 1 < m) ss << " | ";
        }
        ss << std::endl;
      }
#endif

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}

