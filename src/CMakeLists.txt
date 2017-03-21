
add_executable(test_1 test.cpp)
target_link_libraries(test_1 ${pqxx_LIBRARIES})

add_executable(server server/main.cpp)
target_link_libraries(server zmqpp ${pqxx_LIBRARIES})

add_executable(client client/main.cpp)
target_link_libraries(client zmqpp)
