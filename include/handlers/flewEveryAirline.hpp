#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>

namespace jetdb{
	namespace handlers{
		responses::result handle_request(pqxx::work& txn, requests::flewEveryAirline req)
		{
			pqxx::result result = txn.exec(
					"SELECT c.govid FROM client c "
					"WHERE NOT exists "
					"((SELECT a.prefix FROM airline a) "
					" EXCEPT "
					" (SELECT flight.prefix FROM books "
					"    JOIN booked ON booked.bid = books.bid "
					"    JOIN flight ON flight.fid = booked.fid "
					"  WHERE books.govid = c.govid));"
			);

//			txn.commit();
			nlohmann::json jres;
			responses::json_results(jres, result);
			return {true, "", jres};
		}
	}
}
