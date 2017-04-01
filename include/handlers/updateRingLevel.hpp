#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>

namespace jetdb
{
	namespace handlers
	{
		namespace updateRingLevel
		{
			responses::result successMsg = {true, "successfully update user ring level", nullptr};
			responses::result failureMsg = {false, "unable to update user ring level", nullptr};
		}

		responses::result handle_request(pqxx::work &txn, requests::updateRingLevel req)
		{
			txn.conn().prepare("updateRingLevel", "UPDATE loginuser SET ringlevel = $1 WHERE email = $2;");

			try
			{
				pqxx::result result = txn.prepared("updateRingLevel")(req.ringLevel)(req.email).exec();
				if(result.affected_rows() == 0)
				{
					return jetdb::handlers::updateRingLevel::failureMsg;
				}
			} catch(const std::exception &e)
			{
				txn.abort();
				return jetdb::handlers::updateRingLevel::failureMsg;

			}

			return jetdb::handlers::updateRingLevel::successMsg;
		}
	}
}
