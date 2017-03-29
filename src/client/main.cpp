#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

std::string print_table(json data) {
  std::stringstream ss;
  ss << "Number of results: " << data["rows"].size() << std::endl;
  ss << std::endl;
  std::stringstream tt;
  auto columns = data["columns"].get<std::vector<std::string>>();
  int m = columns.size();
  for (int i = 0; i < m; i++)
  {
    ss << std::setw(20) << columns[i];
    for (int j = 0; j < 20; j++)
      tt << "-";

    if (i + 1 < m) {
      ss << " | ";
      tt << "---";
    }
  }

  ss << std::endl;
  ss << tt.str() << std::endl;
  for (auto row : data["rows"]) {
    for (int i = 0; i < m; i++) {
      std::string st = row[columns[i]];
      ss << std::setw(20) << st;
      if (i + 1 < m) ss << " | ";
    }
    ss << std::endl;
  }

  return ss.str();
}

int main(int argc, char** argv) {
  zmqpp::context context;
  zmqpp::socket socket{context, zmqpp::socket_type::request};
  socket.connect("tcp://localhost:5555");

#if 0
  zmqpp::message msg;
  msg << "{\"operation\": \"login\", \"name\": \"susan\", \"password\": \"foobar\"}";
  socket.send(msg);
  socket.receive(msg);
  std::string result;
  msg >> result;
  std::cout << "Client received message: " << result << "\n";
#endif

  for (int i = 1; i < argc; i++) {
    zmqpp::message msg;
    msg << argv[i];
    socket.send(msg);
    socket.receive(msg);
    std::string result;
    msg >> result;
    std::cout << "Client received message: " << result << std::endl;

    json j = json::parse(result);
    if (j.find("reason") != j.end())
      std::cout << std::endl << j["reason"].get<std::string>() << std::endl;

    std::cout << print_table(j["data"]) << std::endl;
  }
  return 0;
}
