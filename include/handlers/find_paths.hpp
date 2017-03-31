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
        "with recursive pathfind(fromairportcode, toairportcode, pathstarttime, pathendtime, last_fid, last_cost, cost, path, fids, cycle, depth) as (select fromairportcode, toairportcode, starttime, endtime, fid, cost, cost, ARRAY[upper(fromairportcode)], ARRAY[fid], false, 0 from flight f union all select f.fromairportcode, f.toairportcode, sf.pathstarttime, f.endtime, f.fid, f.cost, f.cost+sf.cost, path || upper(f.fromairportcode), fids || f.fid, upper(f.fromairportcode) = ANY(path), sf.depth+1 from flight f, pathfind sf where NOT cycle AND f.fromairportcode = sf.toairportcode AND f.starttime > sf.pathendtime), paths as (SELECT fromairportcode, toairportcode, pathstarttime, pathendtime, last_fid,last_cost,cost,path || upper(toairportcode) as complete_path, fids, cycle, depth, path[1] as src, toairportcode as dst FROM pathfind WHERE TRUE OR (NOT cycle AND NOT(toairportcode = ANY(path))) ORDER BY src, dst, cost DESC) select * from paths"
        " where NOT CYCLE and src=$1 and dst=$2;"
      );
      result = txn.prepared("find_paths")(from_airportcode)(to_airportcode).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}

