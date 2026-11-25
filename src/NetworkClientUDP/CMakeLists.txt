file(GLOB_RECURSE SRC "*.cpp" "*.h")
add_executable(NetworkClientUDP ${SRC})
target_link_libraries(NetworkClientUDP PRIVATE sfml-network)
finish_networking_target(NetworkClientUDP)
