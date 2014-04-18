cmake_minimum_required(VERSION 2.8.5)
project(saturn)

#SET(CMAKE_VERBOSE_MAKEFILE ON)
#set(VERBOSE, ON)
#SET(DEBUG, ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeScripts")

# set some output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ensure that the cxx files get compiled with c++11 support enabled
set(CMAKE_CXX_FLAGS "--std=c++11 ${CMAKE_CXX_FLAGS}")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/libsaturn)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libsaturn/include
)

# setup sfml
find_package(SFML COMPONENTS graphics window system)
include_directories(${SFML_INCLUDE_DIR})
#set(SFML_STATIC True)

find_package(OpenGL)

set(THIRD-PARTY ${CMAKE_CURRENT_SOURCE_DIR}/third-party)

set(OPTIONPARSER_DIR ${THIRD-PARTY}/optparse/)
include_directories(${OPTIONPARSER_DIR})
if (TARGET optionparser)
    message(STATUS "OptionParser added as part of another part of Galaxy")
else()
    # setup OptionParser
    add_library(optionparser
        ${OPTIONPARSER_DIR}/OptionParser.cpp
    )
endif()

# add the primary executable
add_executable(saturn
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LEM1802Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SPED3Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard_adaptor.cpp
)

# and link with the required libraries
target_link_libraries(saturn
    libsaturn
    ${SFML_LIBRARY}
    optionparser
    ${OPENGL_LIBRARY}
)

## if testing has been enabled, build the tests and run them! :D
#add_executable(tests
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/test/tests.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/test/test_device.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/test/units/test_disk.cpp
#)
#target_link_libraries(tests libsaturn)

