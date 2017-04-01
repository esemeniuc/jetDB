#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <json.hpp>
#include <algorithm>
#include <future>
#include <thread>

#include <imgui.h>
#include "imgui_impl_sdl.h"
#include <cstdio>
#include <SDL.h>
#include <SDL_opengl.h>

#include <requests.hpp>


struct base_state_t{
  virtual void call() = 0;
  virtual std::unique_ptr<base_state_t> clone() = 0;
  virtual ~base_state_t(){}
};

template<typename State>
struct base_state_impl_t: base_state_t{
  State s;
  base_state_impl_t(State _s): s{std::move(_s)}{}
  void call() override{
    s.call();
  }
  std::unique_ptr<base_state_t> clone() override{
    return std::make_unique<base_state_impl_t<State>>(s);
  }
};

std::vector<std::string> read_sql_list(std::string lst){
  std::vector<std::string> acc;
  std::string tmp;
  for(std::size_t i = 1; i < lst.size()-1; ++i){
    if(lst[i] == ','){
      acc.push_back(tmp);
      tmp = "";
    }else{
      tmp += lst[i];
    }
  }
  acc.push_back(tmp);
  return acc;
}

nlohmann::json get_response_data(nlohmann::json const& js){
  return js[0][0];
}

struct state_t{
  std::unique_ptr<base_state_t> ptr;
  template<typename State>
  state_t(State s):
    ptr{std::make_unique<base_state_impl_t<State>>(std::move(s))}
  {}
  state_t(state_t const& s){
    if(s.ptr){
      ptr = s.ptr->clone();
    }else{
      ptr = nullptr;
    }
  }
  state_t(){}
  void call(){
    ptr->call();
  }
  operator bool(){
    return static_cast<bool>(ptr);
  }
  state_t& operator=(state_t const& other){
    if(other.ptr){
      ptr = other.ptr->clone();
    }else{
      ptr = nullptr;
    }
  }
};

state_t state;
std::string clientGovID;
struct _connection{
  zmqpp::context context;
  zmqpp::socket socket;
  _connection():
    socket{context, zmqpp::socket_type::request}
  {
    socket.connect("tcp://localhost:5555");
  }
  template<typename Request>
  jetdb::responses::result make_request(Request req){
    zmqpp::message msg;
    msg << static_cast<nlohmann::json>(req).dump();
    socket.send(msg);
    socket.receive(msg);
    std::string result;
    msg >> result;
    return nlohmann::json::parse(result);
  }
} connection;

struct failed_state_t{
  state_t next_state;
  std::string reason;
  void call(){
    ImGui::Begin("Error");
    ImGui::Text("%s", reason.c_str());
    if(ImGui::Button("OK")){
      state = next_state;
    }
    ImGui::End();
  }
};

struct dump_display_t{
  state_t next_state;
  std::string data;
  void call(){
    ImGui::Begin("Data");
    ImGui::Text("%s", data.c_str());
    if(ImGui::Button("OK")){
      state = next_state;
    }
    ImGui::End();
  }
};

struct table_display_t{
  state_t next_state;
  nlohmann::json data;
  void call(){
    ImGui::Begin("Data");
    nlohmann::json::array_t columns = data[0][0]["columns"];
    nlohmann::json::array_t rows = data[0][0]["rows"];
    ImGui::Columns(columns.size());
    for(std::string col: columns){
      ImGui::Text("%s", col.c_str());
      ImGui::NextColumn();
    }
    ImGui::Separator();
    for(nlohmann::json::object_t row: rows){
      for(std::size_t i = 0; i < columns.size(); ++i){
        ImGui::Text("%s", row[columns[i].get<std::string>()].get<std::string>().c_str());
        ImGui::NextColumn();
      }
    }
    ImGui::Columns(1);
    if(ImGui::Button("OK")){
      state = next_state;
    }
    ImGui::End();
  }
};

struct query_available_flights_state_t{
  state_t back_state;
  char prefix[256] = {0}, airportcode[256] = {0};
  int flight_id = -1;
  void call(){
    ImGui::Begin("Query Available Flights");
    ImGui::InputText("Prefix", prefix, 256);
    ImGui::InputText("Airport code", airportcode, 256);
    ImGui::InputInt("Flight ID", &flight_id);
    if(ImGui::Button("Execute")){
      auto res = connection.make_request(jetdb::requests::available_flights{prefix, airportcode, flight_id});
      if(res.success){
        state = table_display_t{*this, res.data};
      }else{
        state = failed_state_t{*this, res.reason};
      }
    }
    if(ImGui::Button("Back")){
      state = back_state;
    }
    ImGui::End();
  }
};

struct query_airports_state_t{
  state_t back_state;
  char airportcode[256] = {0}, apname[256] = {0};
  void call(){
    ImGui::Begin("Query Airports");
    ImGui::InputText("Airport code", airportcode, 256);
    ImGui::InputText("Airport name", apname, 256);
    if(ImGui::Button("Execute")){
      auto res = connection.make_request(jetdb::requests::get_airports{airportcode, apname});
      if(res.success){
        state = table_display_t{*this, res.data};
      }else{
        state = failed_state_t{*this, res.reason};
      }
    }
    if(ImGui::Button("Back")){
      state = back_state;
    }
    ImGui::End();
  }
};

struct book_flights_state_t{
  state_t back_state;
  nlohmann::json current_options = nullptr;
  char src_airportcode[256] = {0}, dst_airportcode[256] = {0};
  void call(){
    ImGui::Begin("Book flights");
    ImGui::InputText("From airport", src_airportcode, 256);
    ImGui::InputText("To airport", dst_airportcode, 256);
    if(ImGui::Button("Find Flights")){
      auto res = connection.make_request(jetdb::requests::find_paths{src_airportcode, dst_airportcode});
      auto tmp = book_flights_state_t{back_state, res.data};
      std::copy_n(src_airportcode, 256, tmp.src_airportcode);
      std::copy_n(dst_airportcode, 256, tmp.dst_airportcode);
      state = tmp;
    }

    // booking options
    if(current_options != nullptr && current_options.size() > 0 && current_options.is_array()){
      nlohmann::json _data = current_options[0];
      if(_data != nullptr && _data.is_array() && _data.size() > 0){
        nlohmann::json data = _data[0];
        nlohmann::json::array_t columns = data["columns"];
        nlohmann::json::array_t rows = data["rows"];
        ImGui::Columns(3);
        ImGui::Text("Airports");
        ImGui::NextColumn();
        ImGui::Text("Flights");
        ImGui::NextColumn();
        ImGui::NextColumn();
        ImGui::Separator();
        for(nlohmann::json::object_t row: rows){
          ImGui::Text("%s", row["complete_path"].get<std::string>().c_str());
          ImGui::NextColumn();
          auto fids = read_sql_list(row["fids"]);
          for(std::string fid: fids){
            auto result = connection.make_request(jetdb::requests::available_flights{"", "", std::stoi(fid)});
            nlohmann::json a = result.data[0];
            nlohmann::json b = a[0];
            nlohmann::json c = b["rows"];
            nlohmann::json d = c[0];
            nlohmann::json e = d["fid"];
            std::string fname = e;
            ImGui::Text("%s, ", fname.c_str());
          }
          ImGui::NextColumn();
          if(ImGui::Button("Book")){
            auto result = connection.make_request(jetdb::requests::bookFlight{clientGovID, std::vector<std::string>{}, fids});
            if(result.success){

            }else{
              state = failed_state_t{*this, result.reason};
            }
          }
          ImGui::NextColumn();
        }
      }
      ImGui::Columns(1);
    }

    if(ImGui::Button("Back")){
      state = back_state;
    }
    ImGui::End();
  }
};

/*struct query_airports_state_t{
  state_t back_state;
  char airportcode[256] = {0}, apname[256] = {0};
  void call(){
    ImGui::Begin("Query Airports");
    ImGui::InputText("Airport code", airportcode, 256);
    ImGui::InputText("Airport name", apname, 256);
    if(ImGui::Button("Execute")){
      auto res = connection.make_request(jetdb::requests::get_airports{airportcode, apname});
      if(res.success){
        state = table_display_t{*this, res.data};
      }else{
        state = failed_state_t{*this, res.reason};
      }
    }
    if(ImGui::Button("Back")){
      state = back_state;
    }
    ImGui::End();
  }
};*/

struct query_select_state_t{
  void call(){
    ImGui::Begin("Select Query");
    if(ImGui::Button("List Available Flights")){
      state = query_available_flights_state_t{*this};
    }
    if(ImGui::Button("List Airports")){
      state = query_airports_state_t{*this};
    }
    if(ImGui::Button("Book Flight")){
      state = book_flights_state_t{*this};
    }
    ImGui::Separator();
    if(ImGui::Button("Exit")){
      state = state_t{};
    }
    ImGui::End();
  }
};

struct login_state_t{
  char name[256] = {0}, password[256] = {0};
  void call(){
    ImGui::Begin("Login");
    ImGui::InputText("Name", name, 256);
    ImGui::InputText("Password", password, 256);
    if(ImGui::Button("Login")){
      auto res = connection.make_request(jetdb::requests::login2{name, password});
      if(res.success){
        state = query_select_state_t{};
      }else{
        state = failed_state_t{*this, res.reason};
      }
    }
    if(ImGui::Button("Exit")){
      state = state_t{};
    }
    ImGui::End();
  }
};

int main(int, char**)
{
  // Setup SDL
  if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
  {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  // Setup window
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_DisplayMode current;
  SDL_GetCurrentDisplayMode(0, &current);
  SDL_Window *window = SDL_CreateWindow("jetDB client", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
  SDL_GLContext glcontext = SDL_GL_CreateContext(window);

  // Setup ImGui binding
  ImGui_ImplSdl_Init(window);

  bool show_test_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImColor(114, 144, 154);

  state = login_state_t{};

  // Main loop
  bool done = false;
  while (!done)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      ImGui_ImplSdl_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        done = true;
    }
    ImGui_ImplSdl_NewFrame(window);

    state.call();
    if(!state) done = true;

    // Rendering
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    SDL_GL_SwapWindow(window);
  }

  // Cleanup
  ImGui_ImplSdl_Shutdown();
  SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
