#ifndef __M35FD_TESTS
#define __M35FD_TESTS
#include <m35fd.hpp>

TEST_CASE("hardware/m35fd/initialisation", "test m35fd initailisation") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));
}

TEST_CASE("hardware/m35fd/write_to_image", "test the write to disk mechanism; basically just flashing the disk with info!") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    char* buffer = new char[5];
    for (int i=0; i<5; i++){
        buffer[i] = "hello"[i];
    }
    CAPTURE(buffer);
    m35fd.read_in_image(buffer, 5);
//    REQUIRE(std::strcmp(m35fd.block_image[0], "h"))
}

#endif
