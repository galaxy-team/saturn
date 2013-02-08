cmake_minimum_required(VERSION 2.8.5)
project(saturn)

#set(VERBOSE, True)

set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")

add_library (libsaturn
    src/lib/libsaturn.cpp)



#add_executable(saturn
#    src/main.cpp
#    src/lib/libjupiter.hpp
#    src/lib/libjupiter.cpp
#)

#ADD_TEST(SimpleTest ${CMAKE_CURRENT_BINARY_DIR}/SimpleTest Hello)


