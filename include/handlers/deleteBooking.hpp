#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>

namespace jetdb
{
	namespace handlers
	{
		namespace deleteBooking
		{
			responses::result successMsg = {true, "successfully deleted booking", nullptr};
			responses::result failureMsg = {false, "unable to delete booking", nullptr};
		}

		responses::result handle_request(pqxx::work &txn, requests::deleteBooking req)
		{
			txn.conn().prepare("deleteBooking", "DELETE FROM booking WHERE bid = $1;");

			pqxx::result result = txn.prepared("deleteBooking")(req.bid).exec();
			if(result.affected_rows() > 0)
			{
				txn.commit();
				return jetdb::handlers::deleteBooking::successMsg;
			}
			return jetdb::handlers::deleteBooking::failureMsg;
		}
	}
}
