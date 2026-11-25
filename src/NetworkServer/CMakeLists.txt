file(GLOB_RECURSE SRC "*.cpp" "*.h")
add_executable(NetworkServer ${SRC})
target_link_libraries(NetworkServer PRIVATE sfml-network)
finish_networking_target(NetworkServer)
