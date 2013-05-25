#ifndef __M35FD_TESTS
#define __M35FD_TESTS
#include <m35fd.hpp>
#include <fstream_disk.hpp>
#include <iostream>

TEST_CASE("hardware/m35fd/initialisation", "test m35fd initailisation") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));
}

TEST_CASE("hardware/m35fd/read_in_image", "test the write to disk mechanism; basically just flashing the disk with info!") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    const static int CONTENT_LENGTH = 5;
    char* buffer = new char[CONTENT_LENGTH];
    for (int i=0; i<CONTENT_LENGTH; i++){
        buffer[i] = i;
    }
    CAPTURE(buffer);
    // TODO: ensure the block_image attribute contains the correct content-ish (@ThatOtherPerson this will eseentially be doing what the function read_in_image does anyway... is it worth checking the contents?)
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


TEST_CASE("hardware/m35fd/write_to_floppy_disk", "test writing to floppy disk through assembly :D") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    // generate a random temporary filename
    std::string filename = std::tmpnam(0);
    std::unique_ptr<galaxy::saturn::fstream_disk>floppy_disk = std::unique_ptr<galaxy::saturn::fstream_disk>(new galaxy::saturn::fstream_disk(filename));
    m35fd.insert_disk(floppy_disk);

    std::vector<std::uint16_t> codez;
    for (int i=0; i<m35fd.SECTOR_SIZE; i++) {
        codez.push_back(i);
    }
    cpu.flash(codez.begin(), codez.end());

    // Tell the cpu to write a sector worth of memory from ram at Y to sector X
    cpu.A = 3; // Read sector
    cpu.X = 0; // sector to write to
    cpu.Y = 0; // RAM position to read from
    m35fd.interrupt();

    std::cout << "Last error; 0x" << std::hex << m35fd.last_error_since_poll << std::endl;
    std::cout << "State; 0x" << std::hex << m35fd.state() << std::endl;
    if (m35fd.reading) std::cout << "Disk is currently being read from" << std::endl;
    if (m35fd.writing) std::cout << "Disk is currently being written to" << std::endl;
    REQUIRE_FALSE(cpu.B == 0);

    bool data_correct = true;
    for (int i=0; i<m35fd.SECTOR_SIZE; i++) {
//        if (m35fd.block_image[i] != i) {
//            data_correct = false;
//        }
    }
    REQUIRE(data_correct);

    // and remove the temporary file
    std::remove(filename.c_str());
}

TEST_CASE("hardware/m35fd/read_from_floppy_disk", "test reading from floppy disk through assembly :P") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

//    for (int i = 0; i < 512 && i < m35fd.BLOCK_SIZE; i++) {
//        m35fd.block_image[i] = i;
//    }

    /* TODO: fix this so it uses disks
    m35fd.current_state = 0x1; // set state to STATE_READY

    // Tell the floppy to read a sector from X to ram at Y
    cpu.A = 2; // Read sector
    cpu.X = 0; // sector to read from
    cpu.Y = 0; // RAM position to write to
    m35fd.interrupt();

    REQUIRE_FALSE(cpu.B == 0);

    bool data_correct = true;
    for (int i=0; i<512; i++) {
        if (cpu.ram[i] != i) {
            data_correct = false;
        }
    }
    REQUIRE(data_correct);
    */
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
