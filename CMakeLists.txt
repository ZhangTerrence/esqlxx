cmake_minimum_required(VERSION 3.29)
project(esqlxx)

set(CMAKE_CXX_STANDARD 23)

add_executable(esqlxx main.cpp)

add_subdirectory(lib/libpqxx-7.9.2 build-pqxx)
target_link_libraries(esqlxx PRIVATE pqxx)
