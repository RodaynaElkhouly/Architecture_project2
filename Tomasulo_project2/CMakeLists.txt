cmake_minimum_required(VERSION 3.16)
project(Tomasulo_project2)

set(CMAKE_CXX_STANDARD 17)

add_executable(Tomasulo_project2
        main.cpp
        simulator.cpp
        register_file.cpp
        memory.cpp
)
