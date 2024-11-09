cmake_minimum_required(VERSION 3.29)
project(esqlxx)

set(CMAKE_CXX_STANDARD 23)
option(GENERATE "Generates the code.cpp to be executed." ON)

file(GLOB HEADERS include/*.hpp)
file(GLOB SOURCES src/*.cpp)

if (GENERATE)
    add_executable(esqlxx main.cpp ${HEADERS} ${SOURCES})
else ()
    add_executable(esqlxx code.cpp)
endif ()
unset(GENERATE)

add_subdirectory(lib/libpqxx-7.9.2 build-pqxx)
target_link_libraries(esqlxx PRIVATE pqxx)
