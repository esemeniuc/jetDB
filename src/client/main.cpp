#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <json.hpp>
using json = nlohmann::json;

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
  }
  return 0;
}
