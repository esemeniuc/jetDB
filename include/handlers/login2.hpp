#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
	namespace handlers{
		responses::result handle_request(pqxx::work& txn, requests::login2 req)
		{
			pqxx::result result = txn.prepared("loginValidation")(req.email)(req.password).exec();

			if(result[0][0].as<int>() != 1)
			{
				return {false, "invalid username/password"};
			}
			return {true, "successful authentication"};
		}
	}
}
