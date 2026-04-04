cmake_minimum_required(VERSION 3.20)

project(cpp_chess_engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_GUI "Build the SFML-powered GUI application" ON)
option(BUILD_TESTING_MODULE "Build the command line testing module (PGN + FEN move tests)" ON)

file(GLOB ENGINE_SOURCES "cpp_chess_engine/*.cpp")

list(REMOVE_ITEM ENGINE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp_chess_engine/GUIBoard.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp_chess_engine/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp_chess_engine/pgn_tester_main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp_chess_engine/testing_main.cpp"
)

if(BUILD_GUI)
    include(FetchContent)

    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
    )

    FetchContent_MakeAvailable(sfml)

    add_executable(cpp_chess_engine
        ${ENGINE_SOURCES}
        cpp_chess_engine/GUIBoard.cpp
        cpp_chess_engine/main.cpp
    )

    target_include_directories(cpp_chess_engine PRIVATE cpp_chess_engine)

    target_link_libraries(cpp_chess_engine PRIVATE sfml-graphics sfml-window sfml-system)
endif()

if(BUILD_TESTING_MODULE)
    add_executable(cpp_chess_engine_tests
        ${ENGINE_SOURCES}
        cpp_chess_engine/testing_main.cpp
    )

    target_include_directories(cpp_chess_engine_tests PRIVATE cpp_chess_engine)
endif()

enable_testing()


