/*

This file is part of libsaturn.

libsaturn is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libsaturn is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libsaturn.  If not, see <http://www.gnu.org/licenses/>.

Your copy of the GNU Lesser General Public License should be in the
file named "LICENSE-LGPL.txt".

*/

#include <libsaturn.hpp>
#include <fstream_disk.hpp>

std::array<std::uint16_t, galaxy::saturn::disk::SECTOR_SIZE> galaxy::saturn::fstream_disk::read_sector(std::uint16_t sector)
{
    if (sector >= NUM_SECTORS) {
        throw std::out_of_range("Requested sector does not exist");
    }

    file.seekg(sector * SECTOR_SIZE * 2);
    char buffer[SECTOR_SIZE * 2];
    file.read(buffer, SECTOR_SIZE * 2);

    std::array<std::uint16_t, SECTOR_SIZE> words;

    for (int i = 0; i < SECTOR_SIZE; i++) {
        words[i] = buffer[i * 2] << 0xf | buffer[i * 2 + 1];
    }

    return words;
}

void galaxy::saturn::fstream_disk::write_sector(std::uint16_t sector, std::array<std::uint16_t, galaxy::saturn::disk::SECTOR_SIZE> words)
{
    if (sector >= NUM_SECTORS) {
        throw std::out_of_range("Requested sector does not exist");
    }

    file.seekp(sector * SECTOR_SIZE * 2);
    char bytes[SECTOR_SIZE * 2];

    for (int i = 0; i < SECTOR_SIZE; i++) {
        bytes[i * 2] = words[i] >> 0xf;
        bytes[i * 2 + 1] = words[i] & 0xff;
    }

    file.write(bytes, SECTOR_SIZE);
    file.flush();
}
