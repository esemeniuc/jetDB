cmake_minimum_required(VERSION 3.1.0)

project(jetdb CXX)

set(CMAKE_CXX_STANDARD "14" CACHE INTERNAL "Enable C++14")
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_library(pqxx_LIBRARIES pqxx)
find_path(pqxx_INCLUDES pqxx/pqxx)

include_directories(
  deps/range-v3/include
  deps/zmqpp/src
  deps/catch
  deps/nlohmann_json
  include
  ${pqxx_INCLUDES}
)

add_subdirectory(deps/zmqpp)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
