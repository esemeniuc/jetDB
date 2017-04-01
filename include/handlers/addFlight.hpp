#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>
//#include "../../src/ring2.h"

namespace jetdb
{
	namespace handlers
	{
		namespace addFlight
		{
			responses::result successMsg = {true, "successfully added flight", nullptr};
			responses::result failureMsg = {false, "unable to add flight", nullptr};
		}

		responses::result handle_request(pqxx::work &txn, requests::addFlight req)
		{
			txn.conn().prepare(
					"flightInsert",
					"INSERT INTO Flight (fID, pID, Prefix, FromAirportCode, ToAirportCode, StartTime, EndTime, Cost) "
							"VALUES (DEFAULT, $1, $2, $3, $4, $5, $6, $7);");

			try
			{
				txn.prepared("flightInsert")(req.pID)(req.prefix)(req.fromAirportCode)(req.toAirportCode)
						(req.startTime)(req.endTime)(req.cost).exec();
				return jetdb::handlers::addFlight::successMsg;
			} catch(const std::exception &e)
			{
				txn.abort();
				return jetdb::handlers::addFlight::failureMsg;
			}
		}
	}
}
