#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work& txn, requests::get_bookings req){
      pqxx::result result;
      std::string govid = req.govid;
      txn.conn().prepare(
        "get_bookings",
        "SELECT govid, cname, b.bid, datebooked, b1.cost AS bookcost, "
        "       f.fid, prefix, alname, fromairportcode, toairportcode, "
        "       starttime, endtime, f.cost AS flightcost "
        "FROM booked b "
        "INNER JOIN flight f ON b.fid = f.fid "
        "INNER JOIN books b1 ON b1.bid = b.bid "
        "NATURAL JOIN client "
        "NATURAL JOIN airline "
        "WHERE ($1 = '' OR govid = $1)"
      );
      result = txn.prepared("get_bookings")(govid).exec();

      nlohmann::json jres;
      responses::json_results(jres, result);
      return {true, "", jres};
    }
  }
}
