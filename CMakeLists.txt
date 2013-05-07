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
add_library (libsaturn
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/libsaturn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/device/clock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/device/lem1802.cpp
)
# ensure that the output file for libsaturn only contains one lib (ie, not liblibsaturn)
set_target_properties(libsaturn PROPERTIES PREFIX "")

find_package(SFML COMPONENTS graphics window system)
include_directories(${SFML_INCLUDE_DIR})
#set(SFML_STATIC True)

add_executable(saturn
    src/main.cpp
)
target_link_libraries(saturn ${SFML_LIBRARY})

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

