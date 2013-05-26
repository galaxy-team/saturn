#ifndef __M35FD_TESTS
#define __M35FD_TESTS
#include <m35fd.hpp>
#include <fstream_disk.hpp>
#include <iostream>

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

    // generate a random temporary filename
    std::string filename = std::tmpnam(0);
    CAPTURE(filename);
    m35fd.insert_disk(new galaxy::saturn::fstream_disk(filename));

    std::vector<std::uint16_t> codez;
    for (int i=1; i<m35fd.SECTOR_SIZE; i++) {
        codez.push_back(i);
    }
    cpu.flash(codez.begin(), codez.end());

    // Tell the cpu to write a sector worth of memory from ram at Y to sector X
    cpu.A = 3; // Read sector
    cpu.X = 0; // sector to write to
    cpu.Y = 0; // RAM position to read from
    m35fd.interrupt();

    REQUIRE_FALSE(cpu.B == 0);

    m35fd.eject_disk();

    char buffer[m35fd.SECTOR_SIZE * 2];

    std::ifstream file_obj;
    file_obj.open(filename, std::ios::in | std::ios::binary | std::ios::ate);
    REQUIRE(file_obj.is_open());
    file_obj.read(buffer, m35fd.SECTOR_SIZE * 2);
    file_obj.close();

    bool data_correct = true;
    for (int i=0; i<m35fd.SECTOR_SIZE; i++) {
        if (buffer[i] != i) {
            data_correct = false;
        }
    }
    REQUIRE(data_correct);

    // and remove the temporary file
    std::remove(filename.c_str());
}

TEST_CASE("hardware/m35fd/read_from_floppy_disk", "test reading from floppy disk through assembly :P") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::m35fd& m35fd = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    // generate a random temporary filename
    std::string filename = std::tmpnam(0);
    CAPTURE(filename);

    char buffer[m35fd.SECTOR_SIZE];
    for (int i = 0; i < 512 && i < m35fd.SECTOR_SIZE; i++) {
        buffer[i] = i;
    }

    std::ofstream file_obj;
    file_obj.open(filename, std::ios::out | std::ios::binary | std::ios::ate);
    REQUIRE(file_obj.is_open());
    file_obj.write(buffer, m35fd.SECTOR_SIZE);
    file_obj.close();

    m35fd.insert_disk(new galaxy::saturn::fstream_disk(filename));

    // TODO: fix this so it uses disks

    // Tell the floppy to read a sector from X to ram at Y
    cpu.A = 2; // Read sector
    cpu.X = 0; // sector to read from
    cpu.Y = 0; // RAM position to write to
    m35fd.interrupt();

    REQUIRE_FALSE(cpu.B == 0);

    bool data_correct = true;
    for (int i=0; i<m35fd.SECTOR_SIZE; i++) {
        std::cout << std::hex << cpu.ram[i * 2] << " - " << std::hex << i << std::endl;
        if (cpu.ram[i * 2] != i) {
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
