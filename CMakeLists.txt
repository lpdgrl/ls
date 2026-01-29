cmake_minimum_required(VERSION 3.15)

project(
  ls
  VERSION 0.1
  LANGUAGES CXX)


set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/includes)

set(CPP_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(ls ${CPP_SOURCE}/main.cpp ${CPP_SOURCE}/ls.cpp ${CPP_SOURCE}/lshelper.cpp)

set_property(TARGET ls PROPERTY CXX_STANDARD 11)

