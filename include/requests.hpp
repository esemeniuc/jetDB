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

    // available flights request
    struct available_flights {
      std::string prefix;
      std::string airportcode;
      int fid;
      bool operator==(available_flights const& other) const {
        return std::tie(prefix, airportcode, fid) == std::tie(other.prefix, other.airportcode, other.fid);
      }
    };

    void to_json(nlohmann::json& j, const available_flights& v) {
      j = nlohmann::json{
        {"operation", "available_flights"},
        {"prefix", v.prefix},
        {"airportcode", v.airportcode},
        {"fid", v.fid}
      };
    }

    void from_json(const nlohmann::json& j, available_flights& v) {
      v = available_flights{
        j.find("prefix") != j.end() ? j["prefix"].get<std::string>() : "",
        j.find("airportcode") != j.end() ? j["airportcode"].get<std::string>() : "",
        j.find("fid") != j.end() ? j["fid"].get<int>() : -1
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
		  bool operator==(bookFlight const& other) const{
			  return std::tie(clientGovID, otherPassengerGovIDs, flightIDList) ==
					  std::tie(other.clientGovID, other.otherPassengerGovIDs, other.flightIDList);
		  }
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

	  struct flewEveryAirline{};
	  void to_json(nlohmann::json& j, const flewEveryAirline& v) {
		  j = nlohmann::json{
				  {"operation", "flewEveryAirline"}
		  };
	  }
	  void from_json(const nlohmann::json& j, flewEveryAirline& v) {
		  v = flewEveryAirline{};
	  }

	  struct addFlight{
		  int pID;
		  std::string prefix;
		  std::string fromAirportCode;
		  std::string toAirportCode;
		  std::string startTime;
		  std::string endTime;
		  int cost;

		  bool operator==(addFlight const& other) const{
			  return std::tie(pID, prefix, fromAirportCode, toAirportCode, startTime, endTime, cost) ==
					 std::tie(other.pID, other.prefix, other.fromAirportCode, other.toAirportCode, other.startTime, other.endTime, other.cost);
		  }
	  };
	  void to_json(nlohmann::json& j, const addFlight& v) {
		  j = nlohmann::json{
				  {"operation", "addFlight"},
				  {"pID", v.pID},
				  {"prefix", v.prefix},
				  {"fromAirportCode", v.fromAirportCode},
				  {"toAirportCode", v.toAirportCode},
				  {"startTime", v.startTime},
				  {"endTime", v.endTime},
				  {"cost", v.cost}
		  };
	  }
	  void from_json(const nlohmann::json& j, addFlight& v) {
		  v = addFlight{
				  j["pID"].get<int>(),
				  j["prefix"].get<std::string>(),
				  j["fromAirportCode"].get<std::string>(),
				  j["toAirportCode"].get<std::string>(),
				  j["startTime"].get<std::string>(),
				  j["endTime"].get<std::string>(),
				  j["cost"].get<int>()
		  };
	  }

	  struct addUser{
		  std::string email;
		  std::string password;
		  int ringLevel;

		  bool operator==(addUser const& other) const{
			  return std::tie(email, password, ringLevel) ==
					 std::tie(other.email, other.password, other.ringLevel);
		  }
	  };
	  void to_json(nlohmann::json& j, const addUser& v) {
		  j = nlohmann::json{
				  {"operation", "addUser"},
				  {"email", v.email},
				  {"password", v.password},
				  {"ringLevel", v.ringLevel}
		  };
	  }
	  void from_json(const nlohmann::json& j, addUser& v) {
		  v = addUser{
				  j["email"].get<std::string>(),
				  j["password"].get<std::string>(),
				  j["ringLevel"].get<int>()
		  };
	  }


    // average costs
    struct avg_costs {
      std::string kind;
      bool operator==(avg_costs const& other) const {
        return std::tie(kind) == std::tie(other.kind);
      }
    };

    void to_json(nlohmann::json& j, const avg_costs& v) {
      j = nlohmann::json{
        {"operation", "avg_costs"},
        {"kind", v.kind}
      };
    }

    void from_json(const nlohmann::json& j, avg_costs& v) {
      v = avg_costs{
        j.find("kind") != j.end() ? j["kind"].get<std::string>() : ""
      };
    }

    // find paths
    struct find_paths {
      std::string from_airportcode;
      std::string to_airportcode;
      bool operator==(find_paths const& other) const {
        return std::tie(from_airportcode, to_airportcode) == std::tie(other.from_airportcode, other.to_airportcode);
      }
    };

    void to_json(nlohmann::json& j, const find_paths& v) {
      j = nlohmann::json{
        {"operation", "find_paths"},
        {"from_airportcode", v.from_airportcode},
        {"to_airportcode", v.to_airportcode}
      };
    }

    void from_json(const nlohmann::json& j, find_paths& v) {
      v = find_paths{
        j["from_airportcode"].get<std::string>(),
        j["to_airportcode"].get<std::string>()
      };
    }

    struct cost_summary {
      std::string from_airportcode;
      std::string to_airportcode;
      bool operator==(cost_summary const& other) const {
        return std::tie(from_airportcode, to_airportcode) == std::tie(other.from_airportcode, other.to_airportcode);
      }
    };

    void to_json(nlohmann::json& j, const cost_summary& v) {
      j = nlohmann::json{
        {"operation", "cost_summary"},
        {"from_airportcode", v.from_airportcode},
        {"to_airportcode", v.to_airportcode}
      };
    }

    void from_json(const nlohmann::json& j, cost_summary& v) {
      v = cost_summary{
        j.find("from_airportcode") != j.end() ? j["from_airportcode"].get<std::string>() : "",
        j.find("to_airportcode") != j.end() ? j["to_airportcode"].get<std::string>() : ""
      };
    }
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
        return std::tie(success, reason, data) == std::tie(other.success, other.reason, other.data);
      }
    };
    std::ostream& operator<<(std::ostream& out, result const& s){
      out << static_cast<nlohmann::json>(s);
      return out;
    }
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
