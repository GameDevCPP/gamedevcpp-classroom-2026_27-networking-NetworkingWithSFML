file(GLOB_RECURSE SRC "*.cpp" "*.h")
add_executable(NetworkDemo ${SRC})
target_link_libraries(NetworkDemo PRIVATE sfml-graphics lib_engine)
finish_networking_target(NetworkDemo)
