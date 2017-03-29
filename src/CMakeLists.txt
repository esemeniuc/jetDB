
add_executable(test_1 test.cpp)
add_executable(main main.cpp ring2.cpp ring2.h)
target_link_libraries(test_1 ${pqxx_LIBRARIES})
target_link_libraries(main ${pqxx_LIBRARIES})

add_executable(server server/main.cpp)
target_link_libraries(server zmqpp ${pqxx_LIBRARIES})

add_executable(client client/main.cpp)
target_link_libraries(client zmqpp)
