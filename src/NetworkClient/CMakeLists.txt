file(GLOB_RECURSE SRC "*.cpp" "*.h")
add_executable(NetworkClient ${SRC})
target_link_libraries(NetworkClient PRIVATE sfml-network)
finish_networking_target(NetworkClient)
