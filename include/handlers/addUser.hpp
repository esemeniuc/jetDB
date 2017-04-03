#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>

namespace jetdb
{
	namespace handlers
	{
		namespace addUser
		{
			responses::result successMsg = {true, "successfully added user", nullptr};
			responses::result failureMsg = {false, "unable to add user", nullptr};
		}

		responses::result handle_request(pqxx::work &txn, requests::addUser req)
		{
			txn.conn().prepare(
					"userInsert",
					"INSERT INTO LoginUser (Email, Password, RingLevel) "
							"VALUES ($1, $2, $3);");

			try
			{
				txn.prepared("userInsert")(req.email)(req.password)(req.ringLevel).exec();
				txn.commit();
				return jetdb::handlers::addUser::successMsg;
			} catch(const std::exception &e)
			{
				txn.abort();
				return jetdb::handlers::addUser::failureMsg;
			}
		}
	}
}
