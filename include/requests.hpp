#pragma once

#include <string>
#include <tuple>
#include <pqxx/pqxx>
#include <json.hpp>

namespace jetdb{
  namespace requests{
    // login request
    struct login{
      std::string name;
      std::string password;
      bool operator==(login const& other) const{
        return std::tie(name, password) == std::tie(other.name, other.password);
      }
    };
    void to_json(nlohmann::json& j, const login& v) {
      j = nlohmann::json{
        {"operation", "login"},
        {"name", v.name},
        {"password", v.password}
      };
    }
    void from_json(const nlohmann::json& j, login& v) {
      v = login{
        j["name"].get<std::string>(),
        j["password"].get<std::string>()
      };
    }

#if 0
	  //cwaffles testing stuff
	  struct login2{
		  std::string email;
		  std::string password;
		  bool operator==(login2 const& other) const{
			  return std::tie(email, password) == std::tie(other.email, other.password);
		  }
	  };
	  void to_json(nlohmann::json& j, const login2& v) {
		  j = nlohmann::json{
				  {"operation", "login2"},
				  {"email", v.email},
				  {"password", v.password}
		  };
	  }
	  void from_json(const nlohmann::json& j, login2& v) {
		  v = login2{
				  j["email"].get<std::string>(),
				  j["password"].get<std::string>()
		  };
	  }
#endif

    // available flights request
    struct available_flights {
      std::string prefix;
      std::string airportcode;
      bool operator==(available_flights const& other) const{
        return std::tie(prefix, airportcode) == std::tie(other.prefix, other.airportcode);
      }
    };

    void to_json(nlohmann::json& j, const available_flights& v) {
      j = nlohmann::json{
        {"operation", "available_flights"},
        {"prefix", v.prefix},
        {"airportcode", v.airportcode}
      };
    }

    void from_json(const nlohmann::json& j, available_flights& v) {
      v = available_flights{
        j.find("prefix") != j.end() ? j["prefix"].get<std::string>() : "",
        j.find("airportcode") != j.end() ? j["airportcode"].get<std::string>() : ""
      };
    }


    // get airports
    struct get_airports {
      std::string airportcode;
      std::string apname;
      bool operator==(get_airports const& other) const {
        return std::tie(airportcode, apname) == std::tie(other.airportcode, other.apname);
      }
    };

    void to_json(nlohmann::json& j, const get_airports& v) {
      j = nlohmann::json{
        {"operation", "get_airports"},
        {"airportcode", v.airportcode},
        {"apname", v.apname}
      };
    }

    void from_json(const nlohmann::json& j, get_airports& v) {
      v = get_airports{
        j.find("airportcode") != j.end() ? j["airportcode"].get<std::string>() : "",
        j.find("apname") != j.end() ? j["apname"].get<std::string>() : ""
      };
    }

    // get airlines
    struct get_airlines {
      std::string prefix;
      std::string alname;
      bool operator==(get_airlines const& other) const {
        return std::tie(prefix, alname) == std::tie(other.prefix, other.alname);
      }
    };

    void to_json(nlohmann::json& j, const get_airlines& v) {
      j = nlohmann::json{
        {"operation", "get_airlines"},
        {"prefix", v.prefix},
        {"alname", v.alname}
      };
    }

    void from_json(const nlohmann::json& j, get_airlines& v) {
      v = get_airlines{
        j.find("prefix") != j.end() ? j["prefix"].get<std::string>() : "",
        j.find("alname") != j.end() ? j["alname"].get<std::string>() : ""
      };
    }

#if 0
	  //cwaffles testing stuff
	  struct login2{
		  std::string email;
		  std::string password;
		  bool operator==(login2 const& other) const{
			  return std::tie(email, password) == std::tie(other.email, other.password);
		  }
	  };
	  void to_json(nlohmann::json& j, const login2& v) {
		  j = nlohmann::json{
				  {"operation", "login2"},
				  {"email", v.email},
				  {"password", v.password}
		  };
	  }
	  void from_json(const nlohmann::json& j, login2& v) {
		  v = login2{
				  j["email"].get<std::string>(),
				  j["password"].get<std::string>()
		  };
	  }


	  struct bookFlight{
		  std::string clientGovID;
		  std::vector<std::string> otherPassengerGovIDs;
		  std::vector<std::string> flightIDList;
	  };
	  void to_json(nlohmann::json& j, const bookFlight& v) {
		  j = nlohmann::json{
				  {"operation", "bookFlight"},
				  {"clientGovID", v.clientGovID},
				  {"otherPassengerGovIDs", v.otherPassengerGovIDs},
				  {"flightIDs", v.flightIDList}
		  };
	  }
	  void from_json(const nlohmann::json& j, bookFlight& v) {
		  v = bookFlight{
				  j["clientGovID"].get<std::string>(),
				  j["otherPassengerGovIDs"].get<std::vector<std::string>>(),
				  j["flightIDs"].get<std::vector<std::string>>()
		  };
	  }
#endif

  }

  namespace responses{
    struct result{
      bool success;
      std::string reason;
      nlohmann::json data;

      result() = default;
      result(bool s, std::string reason="", nlohmann::json data={}):
        success{s},
        reason{std::move(reason)},
        data{std::move(data)}
      {}
      result(std::exception const& e):
        success{false},
        reason{e.what()},
        data{}
      {}
      bool operator==(result const& other) const{
        return std::tie(success, reason) == std::tie(other.success, other.reason);
      }
    };
    /*std::ostream& operator<<(std::ostream& out, success const& s){
      out << "success{" << s.succeeded << ",\"" << s.reason << "\"}";
      return out;
    }*/
    void to_json(nlohmann::json& j, const result& v) {
      j = nlohmann::json{
        {"success", v.success},
        {"reason", v.reason},
        {"data", v.data}
      };
    }
    void from_json(const nlohmann::json& j, result& v) {
      v = result{
        j["success"].get<bool>(),
        j["reason"].get<std::string>(),
        j["data"]
      };
    }

    void json_results(nlohmann::json& j, pqxx::result& result) {
      int m = result.columns();
      std::vector<std::string> columns(m);

      nlohmann::json items = nlohmann::json::array();
      for(pqxx::tuple_size_type i = 0; i < m; i++)
        columns[i] = result.column_name(i);

      j["columns"] = columns;

      for (auto row : result) {
        nlohmann::json item = nlohmann::json::object();
        for (pqxx::tuple_size_type i = 0; i < m; i++) {
          item[columns[i]] = row[i].c_str();
        }
        items.push_back(item);
      }

      j["rows"] = items;
    }
  }
}
