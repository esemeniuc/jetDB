#pragma once

#include <pqxx/pqxx>
#include "handlers.hpp"
namespace jetdb{
	void prepare(pqxx::connection_base& conn){
		// have fun
		conn.prepare("loginValidation", "SELECT COUNT(*) FROM LoginUser WHERE Email = $1 AND Password = $2");
//		conn.prepare("validationString", "SELECT COUNT(*) FROM LoginUser WHERE Email = $1 AND Password = $2");

	}
}
