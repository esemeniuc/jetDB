#include <zmqpp/zmqpp.hpp>
#include <pqxx/pqxx>
#include <iostream>
#include <handlers.hpp>
#include <json.hpp>

int main(){
  zmqpp::context context;
  zmqpp::socket socket{context, zmqpp::socket_type::reply};
  pqxx::connection connection{"host=localhost user=postgres"};
  socket.bind("tcp://*:5555");

  zmqpp::message msg;
  while(true){
    socket.receive(msg);
    std::string contents;
    msg >> contents;
    std::cout << "Server received message: " << contents << "\n";
    {
      pqxx::work txn{connection};
      zmqpp::message response;
      response << jetdb::handlers::handle_request(txn, nlohmann::json::parse(contents)).dump();
      socket.send(response);
    }
  }
  return 0;
}
