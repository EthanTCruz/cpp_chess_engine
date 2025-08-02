cmake_minimum_required(VERSION 3.20)

project(cpp_chess_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(sfml)

file(GLOB SOURCE_FILES "cpp_chess_engine/*.cpp")

add_executable(cpp_chess_engine ${SOURCE_FILES})

target_include_directories(cpp_chess_engine PRIVATE cpp_chess_engine)

target_link_libraries(cpp_chess_engine PRIVATE sfml-graphics sfml-window sfml-system)

