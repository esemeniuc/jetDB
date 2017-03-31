#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::find_paths req){
      pqxx::result result;
      std::string from_airportcode = req.from_airportcode;
      std::string to_airportcode = req.to_airportcode;
      txn.conn().prepare(
        "find_paths",
        "WITH RECURSIVE pathfind(fromairportcode, toairportcode, last_fid, last_cost, cost, path, fids, cycle, depth)"
        " as ("
        "SELECT fromairportcode, toairportcode, fid, cost, cost, ARRAY[upper(fromairportcode)], ARRAY[fid], false, 0 from flight f"
        " UNION ALL "
        "SELECT f.fromairportcode, f.toairportcode, f.fid, f.cost, f.cost+sf.cost,"
        " path || upper(f.fromairportcode), fids || f.fid, upper(f.fromairportcode) = ANY(path), sf.depth+1"
        " FROM flight f, pathfind sf "
        "WHERE NOT cycle AND f.fromairportcode = sf.toairportcode"
        "), paths AS ("
        "SELECT fromairportcode,toairportcode,last_fid,last_cost,cost,path || upper(toairportcode) as complete_path,"
        " fids,cycle,depth, path[1] as src, toairportcode as dst"
        " FROM pathfind ORDER BY src, dst, cost DESC)"
        " select * from paths where NOT CYCLE and src=$1 and dst=$2;"
      );
      result = txn.prepared("find_paths")(from_airportcode)(to_airportcode).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}

