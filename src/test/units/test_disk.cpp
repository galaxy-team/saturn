#include "test_disk.hpp"


std::array<std::uint16_t, test_disk::SECTOR_SIZE> test_disk::read_sector(std::uint16_t s)
{
    last_requested_sector = s;
    return sector;
}

void test_disk::write_sector(std::uint16_t s, std::array<std::uint16_t, test_disk::SECTOR_SIZE> words)
{
    last_requested_sector = s;
    sector = words;
}
