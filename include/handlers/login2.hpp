#pragma once

#include <pqxx/pqxx>
#include <requests.hpp>

namespace jetdb{
	namespace handlers{
		responses::result handle_request(pqxx::work& txn, requests::login2 req)
		{
			txn.conn().prepare("loginValidation", "SELECT COUNT(*) FROM LoginUser WHERE Email = $1 AND Password = "
					"$2");

			pqxx::result result = txn.prepared("loginValidation")(req.email)(req.password).exec();

//			txn.commit();

			if(result[0][0].as<int>() != 1)
			{
				return {false, "invalid username/password"};
			}
			return {true, "successful authentication"};
		}
	}



}
