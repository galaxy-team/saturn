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

# ensure the linker can find libsaturn
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lib/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lib/device)

# create a standalone library file for integration
set(SATURN_LIB_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/)
add_library (libsaturn
    ${SATURN_LIB_SRC}/libsaturn.cpp
    ${SATURN_LIB_SRC}/device/clock.cpp
    ${SATURN_LIB_SRC}/device/keyboard.cpp
    ${SATURN_LIB_SRC}/device/lem1802.cpp
    ${SATURN_LIB_SRC}/device/m35fd.cpp
)
# ensure that the output file for libsaturn only contains one lib (ie, not liblibsaturn)
set_target_properties(libsaturn PROPERTIES PREFIX "")

# setup sfml
find_package(SFML COMPONENTS graphics window system)
include_directories(${SFML_INCLUDE_DIR})
#set(SFML_STATIC True)

# setup OptionParser
set(OPTIONPARSER_DIR ${THIRD-PARTY}/optparse/)
add_library(optionparser
    ${OPTIONPARSER_DIR}/OptionParser.cpp
)
include_directories(${OPTIONPARSER_DIR})

# add the primary executable
add_executable(saturn
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LEM1802Window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/keyboard_adaptor.cpp
)

# and link with the required libraries
target_link_libraries(saturn
    libsaturn
    ${SFML_LIBRARY}
    optionparser
)

## if testing has been enabled, build the tests and run them! :D
set(TEST "YES")
IF (TEST)
    ENABLE_TESTING()

    add_executable(tests
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test/tests.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test/test_device.cpp
    )
    target_link_libraries(tests libsaturn)

    ADD_TEST(tests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/tests "Unit tests")
ENDIF(TEST)

