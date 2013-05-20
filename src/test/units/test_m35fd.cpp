#ifndef __M35FD_TESTS
#define __M35FD_TESTS
#include <m35fd.hpp>
#include <iostream>

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

TEST_CASE("hardware/m35fd/write_to_floppy_disk", "test writing to floppy disk through assembly :D") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));
 
    std::vector<std::uint16_t> codez;
    for (int i=0; i<512; i++) {
        codez.push_back(i);
    }
    cpu.flash(codez.begin(), codez.end());
    // TODO: check
}

TEST_CASE("hardware/m35fd/read_from_floppy_disk", "test reading from floppy disk through assembly :P") { 
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    char * buffer = new char[512];
    for (int i=0; i<512; i++) {
        buffer[i] = i;
    }
    m35fd.read_in_image(buffer, 512);
    m35fd.current_state = 0x1; // set state to STATE_READY

    // Tell the floppy to read a sector from X to ram at Y
    cpu.A = 2;
    cpu.X = 0; // sector 0
    cpu.Y = 0;
    m35fd.interrupt();

    bool good = true;
    for (int i=0; i<512; i++) {
        REQUIRE(cpu.ram[i] == i);
        //if (!cpu.ram[i] != i) {
          //  good = false;
        //}
    }
//    REQUIRE(good);
}

TEST_CASE("hardware/m35fd/test_default_state", "tests the default state of the floppy") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    bool good = true;
    for (int i=0; i < m35fd.BLOCK_SIZE; i++) {
        if (!m35fd.block_image[i] == 0x0) {
            good = false;
        }
    }
    REQUIRE(good);
}

#endif
