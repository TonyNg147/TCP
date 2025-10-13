cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

add_executable(Server server.cpp)
add_executable(Client client.cpp)