
add_executable(tests tests.cpp)
target_link_libraries(tests ${pqxx_LIBRARIES})

add_test(NAME tests COMMAND ./playground.sh $<TARGET_FILE:tests> WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
#add_test(NAME tests COMMAND bash -c './tests')
