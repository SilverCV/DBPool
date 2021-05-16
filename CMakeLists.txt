cmake_minimum_required(VERSION 3.17)
project(DBPool)

set(CMAKE_CXX_STANDARD 14)

add_executable(${PROJECT_NAME} main.cpp DBPool.h DBPool.cpp)
target_link_libraries(${PROJECT_NAME} pthread mysqlclient)