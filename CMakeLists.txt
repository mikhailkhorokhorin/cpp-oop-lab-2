cmake_minimum_required(VERSION 3.10.0)
project(CPP-OOP-LABS VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src/include)

add_executable(lab1
    main.cpp
    src/solution.cpp
)