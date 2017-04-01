
#add_executable(ring2test test.cpp ring2.cpp ring2.h utility.cpp utility.h)
#add_executable(ring2main main.cpp ring2.cpp ring2.h utility.cpp utility.h)
#target_link_libraries(ring2test ${pqxx_LIBRARIES})
#target_link_libraries(ring2main ${pqxx_LIBRARIES})

add_executable(server server/main.cpp)
target_link_libraries(server zmqpp ${pqxx_LIBRARIES})

add_executable(client client/main.cpp)
target_link_libraries(client zmqpp)

add_executable(client2 client/main2.cpp ${IMGUI_SRCS} client/imgui_impl_sdl.cpp)
target_link_libraries(client2 zmqpp ${SDL2_LIBRARIES} ${SDL2_LDFLAGS} ${OPENGL_LIBRARIES} ${pqxx_LIBRARIES})
target_compile_options(client2 PUBLIC ${SDL2_CFLAGS})
