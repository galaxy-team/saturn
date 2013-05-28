#ifndef TEST_DISK_HPP
#define TEST_DSIK_HPP

#include <libsaturn.hpp>

class test_disk : public galaxy::saturn::disk {
    public:
        std::array<uint16_t, SECTOR_SIZE> sector;
        int last_requested_sector;

        virtual std::array<std::uint16_t, SECTOR_SIZE> read_sector(std::uint16_t);
        virtual void write_sector(std::uint16_t, std::array<std::uint16_t, SECTOR_SIZE>);
};

#endif // TEST_DEVICE_HPP
