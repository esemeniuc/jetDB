#include <zmqpp/zmqpp.hpp>
#include <iostream>

int main(){
  zmqpp::context context;
  zmqpp::socket socket{context, zmqpp::socket_type::request};
  socket.connect("tcp://localhost:5555");

  zmqpp::message msg;
  msg << "{\"operation\": \"login\", \"name\": \"susan\", \"password\": \"foobar\"}";
  socket.send(msg);
  socket.receive(msg);
  std::string result;
  msg >> result;
  std::cout << "Client received message: " << result << "\n";
  return 0;
}
