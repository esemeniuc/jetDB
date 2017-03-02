cmake_minimum_required(VERSION 3.1.0)

project(jetdb CXX)

set(CMAKE_CXX_STANDARD "14" CACHE INTERNAL "Enable C++14")

find_library(pqxx_LIBRARIES pqxx)
find_path(pqxx_INCLUDES pqxx/pqxx)
find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)

include_directories(
  deps/range-v3/include
  deps/catch
  include
  ${pqxx_INCLUDES}
)

enable_testing()

add_subdirectory(src)
