link_directories(${GTKMM_LIBRARY_DIRS})
include_directories(${GTKMM_INCLUDE_DIRS})

#test_1
add_executable(test_1 test.cpp)
target_link_libraries(test_1 ${pqxx_LIBRARIES})

#main
add_executable(main main.cpp ring2.cpp ring2.h ring2Window.cpp ring2Window.h)
target_link_libraries(main ${pqxx_LIBRARIES} ${GTKMM_LIBRARIES})
