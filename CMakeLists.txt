cmake_minimum_required(VERSION 2.8.5)
project(saturn)

#SET(CMAKE_VERBOSE_MAKEFILE ON)
#set(VERBOSE, ON)
#SET(DEBUG, ON)

# set some output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ensure that the cxx files get compiled with c++11 support enabled
set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")

# ensure the linker can find libsaturn
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lib/)

# create a standalone library file for integration
add_library (libsaturn
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/libsaturn.cpp
)
# ensure that the output file for libsaturn only contains one lib
set_target_properties(libsaturn PROPERTIES PREFIX "")

# build the assembler frontend executable
#add_executable(saturn
#    src/main.cpp
#)

# if testing has been enabled, build the tests and run them! :D
IF (TEST)
    # turns out you have to manually build tests before you can run them. go figure
    add_executable(test_basic_opcodes
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test/basic_opcodes.cpp
    )

    ENABLE_TESTING()
    ADD_TEST(basic_opcodes ${CMAKE_CURRENT_BINARY_DIR}/basic_opcodes "Basic opcodes")
ENDIF(TEST)

