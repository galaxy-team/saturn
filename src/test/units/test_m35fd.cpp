#ifndef __M35FD_TESTS
#define __M35FD_TESTS
#include <iostream>
#include "test_disk.hpp"

TEST_CASE("hardware/m35fd/initialisation", "test m35fd initailisation") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));
}

/*
NOTE: these two tests have been rendered obselete
TEST_CASE("hardware/m35fd/read_in_image", "test the write to disk mechanism; basically just flashing the disk with info!") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    const static int CONTENT_LENGTH = 5;
    char* buffer = new char[CONTENT_LENGTH];
    for (int i=0; i<CONTENT_LENGTH; i++){
        buffer[i] = i;
    }
    CAPTURE(buffer);
    // TODO: chec
}


TEST_CASE("hardware/m35fd/write_out_image", "test the read from disk mechanism; basically just loading the contents of the disk into a buffer") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    char* buffer = new char[5];
    for (int i=0; i<5; i++){
        buffer[i] = i;
    }
    // TODO: see previous test TODO
}
*/

TEST_CASE("hardware/m35fd/write_to_floppy_disk", "test writing to floppy disk through assembly :D") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    test_disk& disk = static_cast<test_disk&>(m35fd.insert_disk(new test_disk()));

    std::vector<std::uint16_t> codez;
    for (int i=0; i<m35fd.SECTOR_SIZE; i++) {
        codez.push_back(i);
    }
    cpu.flash(codez.begin(), codez.end());

    // Tell the cpu to write a sector worth of memory from ram at Y to sector X
    cpu.A = 3; // Read sector
    cpu.X = 0xf0; // sector to write to
    cpu.Y = 0; // RAM position to read from
    m35fd.interrupt();

    REQUIRE_FALSE(cpu.B == 0);
    REQUIRE(m35fd.state() == m35fd.STATE_BUSY);

    bool data_correct = true;
    for (int i = 0; i < m35fd.SECTOR_SIZE; i++) {
        if (disk.sector[i] != codez[i]) {
            data_correct = false;
        }
    }

    REQUIRE(data_correct);
    REQUIRE(disk.last_requested_sector == 0xf0);
}


TEST_CASE("hardware/m35fd/read_from_floppy_disk", "test reading from floppy disk through assembly :P") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    test_disk& disk = static_cast<test_disk&>(m35fd.insert_disk(new test_disk()));

    for (int i = 0; i < disk.SECTOR_SIZE; i++) {
        disk.sector[i] = i;
    }

    // Tell the floppy to read a sector from X to ram at Y
    cpu.A = 2; // Read sector
    cpu.X = 0x123; // sector to read from
    cpu.Y = 0; // RAM position to write to
    m35fd.interrupt();

    REQUIRE_FALSE(cpu.B == 0);
    REQUIRE(m35fd.state() == m35fd.STATE_BUSY);

    // twenty times, to make quite sure
    // TODO: actually test the timing
    for (int i = 0; i < 3900; i++) {
        m35fd.cycle();
    }
    REQUIRE(m35fd.state() == m35fd.STATE_READY);

    // make sure the m35fd read the correct sector;
    REQUIRE(disk.last_requested_sector == 0x123);

    bool data_correct = true;
    for (int i=0; i < m35fd.SECTOR_SIZE; i++) {
        if (cpu.ram[i] != disk.sector[i]) {
            data_correct = false;
        }
    }
    REQUIRE(data_correct);
}
/*
TODO: figure out why this test only fails sometimes...
TEST_CASE("hardware/m35fd/test_default_state", "tests the default state of the floppy") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    bool correct_values = true;
    for (int i=0; i < m35fd.BLOCK_SIZE; i++) {
        if (m35fd.block_image[i] != 0x0) {
            correct_values = false;
        }
    }
    REQUIRE(correct_values);
}
*/
#endif
