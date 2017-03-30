#pragma once

#include <requests.hpp>
#include <pqxx/pqxx>

namespace jetdb{
  namespace handlers{
    responses::result handle_request(pqxx::work&, requests::login2 req){
      if(req.email == "test@test.com" && req.password == "password")
      {
        return {true, "super secret"};
      }
      return {false, "not secret"};
    }
  }
}
