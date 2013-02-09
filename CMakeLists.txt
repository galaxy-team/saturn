cmake_minimum_required(VERSION 2.8.5)
project(saturn)

#set(VERBOSE, True)

set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")

add_library (saturn
    src/lib/libsaturn.cpp)

# these commented out lines will be enabled as the corresponding features are written

#add_executable(saturn
#    src/main.cpp
#)
#target_link_libraries(saturn
#     libsaturn
#)

#ENABLE_TESTING()
#ADD_TEST(SimpleTest ${CMAKE_CURRENT_BINARY_DIR}/SimpleTest Hello)


