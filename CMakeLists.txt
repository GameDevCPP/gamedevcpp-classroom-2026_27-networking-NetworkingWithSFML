cmake_minimum_required(VERSION 3.21)
project(CMakeSFMLProject LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    # Shallow: a full clone of SFML's history is slow and, being a much
    # longer-running transfer, noticeably more likely to get hit by a
    # transient network blip than a one-commit fetch.
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(SFML)

# Dear ImGui + ImGui-SFML, optional, wired into the engine loop already.
# Pinned to v1.89: the version imgui-sfml's 2.6.x branch is tested against.
# Newer ImGui changed the texture-ID API and won't compile against it.
FetchContent_Declare(imgui
    URL "https://github.com/ocornut/imgui/archive/v1.89.zip")
FetchContent_MakeAvailable(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
add_compile_definitions(IMGUI_SFML_DISABLE_OBSOLETE_FUNCTIONS)
FetchContent_Declare(imgui-sfml
    GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
    GIT_TAG 2.6.x
    GIT_SHALLOW TRUE
    # imgui-sfml 2.6.x (maintenance-only, per its own README) predates SFML's
    # early-2024 U8StringCharTraits change, so sf::Clipboard::getString()'s
    # return type no longer implicitly converts the way this line assumes.
    # Fix via a CMake script doing a literal string replace, not git apply:
    # a context-diff patch is fragile against CRLF checkouts on Windows
    # (core.autocrlf=true), which makes "patch does not apply" show up on
    # some machines but not others depending on their git config.
    PATCH_COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/patches/fix-clipboard-u8string.cmake)
FetchContent_MakeAvailable(imgui-sfml)

# B2D - Box2D physics library (unused by this practical, but the engine's
# system_physics links against it either way)
add_subdirectory("lib/b2d/Box2D")
set(B2D_INCS "lib/b2d/Box2D/")

file(GLOB_RECURSE RESOURCE_FILES "res/*.*")
file(GLOB_RECURSE SOURCE_FILES engine/*.cpp engine/*.h)
add_library(lib_engine STATIC ${SOURCE_FILES} ${RESOURCE_FILES})
target_include_directories(lib_engine INTERFACE "${CMAKE_SOURCE_DIR}/engine/" PUBLIC SYSTEM ${B2D_INCS})
target_link_libraries(lib_engine lib_tile_level_loader lib_maths lib_ecm Box2D sfml-graphics sfml-audio sfml-network ImGui-SFML::ImGui-SFML)

file(GLOB_RECURSE SOURCE_FILES lib_tile_level_loader/*.cpp lib_tile_level_loader/*.h)
add_library(lib_tile_level_loader STATIC ${SOURCE_FILES})
target_include_directories(lib_tile_level_loader INTERFACE "${CMAKE_SOURCE_DIR}/lib_tile_level_loader/" )
target_link_libraries(lib_tile_level_loader lib_maths sfml-graphics)

add_library(lib_maths INTERFACE)
target_sources(lib_maths INTERFACE "${CMAKE_SOURCE_DIR}/lib_maths/maths.h")
target_include_directories(lib_maths INTERFACE "${CMAKE_SOURCE_DIR}/lib_maths")
target_link_libraries(lib_maths INTERFACE sfml-system)

file(GLOB_RECURSE SOURCE_FILES lib_ecm/*.cpp lib_ecm/*.h)
add_library(lib_ecm STATIC ${SOURCE_FILES})
target_include_directories(lib_ecm INTERFACE "${CMAKE_SOURCE_DIR}/lib_ecm" )
target_link_libraries(lib_ecm PRIVATE lib_maths)

add_custom_target(copy_resources ALL COMMAND ${CMAKE_COMMAND}
        -E copy_directory
        "${PROJECT_SOURCE_DIR}/res"
        "${CMAKE_BINARY_DIR}/res"
)

#### Subprojects: each is a real separate CMake project in its own
#### src/<Name>/CMakeLists.txt. Each defines its own executable and links
#### it -- that part is what's actually theirs. This function only wires
#### up the plumbing that's identical for all of them.
function(finish_networking_target name)
    target_compile_features(${name} PRIVATE cxx_std_17)
    add_dependencies(${name} copy_resources)
    # add_subdirectory() would otherwise land each binary under
    # build/src/<name>/, away from copy_resources' build/res.
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
    install(TARGETS ${name})
    if (WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${name}> $<TARGET_FILE_DIR:${name}> COMMAND_EXPAND_LISTS)
    endif()
endfunction()

# The full demo: host-and-join over TCP, component-based (ECM). A real
# window, two circles, move one with keys, watch the network-driven one
# follow.
add_subdirectory(src/NetworkDemo)

# Bare console TCP, no ECM -- matches the "Starting a Basic Server"/
# "Starting a Basic Client"/"Sending and Receiving" steps.
add_subdirectory(src/NetworkServer)
add_subdirectory(src/NetworkClient)

# Bare console UDP, no ECM -- matches the "Adding a UDP Server"/"Adding a
# UDP Client" steps.
add_subdirectory(src/NetworkServerUDP)
add_subdirectory(src/NetworkClientUDP)
