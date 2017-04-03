#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <json.hpp>
#include <algorithm>

using json = nlohmann::json;

std::string print_table(json data) {
  if(data == nullptr) return "no data";
  std::stringstream ss;
  ss << "Number of results: " << data["rows"].size() << std::endl;
  ss << std::endl;
  std::stringstream tt;
  auto columns = data["columns"].get<std::vector<std::string>>();
  int m = columns.size();

  std::vector<unsigned int> widths(m);
  for (int i = 0; i < m; i++)
  {
    widths[i] = std::max((unsigned int) 20, (unsigned int) columns[i].length());
    for (auto row : data["rows"]) {
      std::string st = row[columns[i]];
      widths[i] = std::max(widths[i], (unsigned int) st.length());
    }
  }

  for (int i = 0; i < m; i++)
  {
    int w = widths[i];
    ss << std::setw(w) << columns[i];
    for (int j = 0; j < w; j++)
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
      ss << std::setw(widths[i]) << st;
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
