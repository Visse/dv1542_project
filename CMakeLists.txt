cmake_minimum_required(VERSION 2.6)
project(dv1542_project)

add_executable(dv1542_project main.cpp)

install(TARGETS dv1542_project RUNTIME DESTINATION bin)
