file(GLOB_RECURSE SRC "*.cpp" "*.h")
add_executable(NetworkServerUDP ${SRC})
target_link_libraries(NetworkServerUDP PRIVATE sfml-network)
finish_networking_target(NetworkServerUDP)
