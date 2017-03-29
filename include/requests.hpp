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
  }

  namespace responses{
    struct result{
      bool success;
      std::string reason;

      result() = default;
      result(bool s, std::string reason=""):
        success{s},
        reason{std::move(reason)}
      {}
      result(std::exception const& e):
        success{false},
        reason{e.what()}
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
        {"reason", v.reason}
      };
    }
    void from_json(const nlohmann::json& j, result& v) {
      v = result{
        j["success"].get<bool>(),
        j["reason"].get<std::string>()
      };
    }

    void json_results(nlohmann::json& j, pqxx::result& result) {
      int m = result.columns();
      std::vector<std::string> columns(m);
      for(pqxx::tuple_size_type i = 0; i < m; i++)
        columns[i] = result.column_name(i);

      for (auto row : result) {
        nlohmann::json item = nlohmann::json::object();
        for (pqxx::tuple_size_type i = 0; i < m; i++) {
          item[columns[i]] = row[i].c_str();
        }
        j.push_back(item);
      }
    }
  }
}
