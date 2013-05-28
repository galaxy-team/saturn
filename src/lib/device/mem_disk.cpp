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
#include <mem_disk.hpp>

std::array<std::uint16_t, galaxy::saturn::disk::SECTOR_SIZE> galaxy::saturn::mem_disk::read_sector(std::uint16_t sector)
{
    if (sector >= NUM_SECTORS) {
        throw std::out_of_range("Requested sector does not exist");
    }
    
    return sectors[sector];
}

void galaxy::saturn::mem_disk::write_sector(std::uint16_t sector, std::array<std::uint16_t, galaxy::saturn::disk::SECTOR_SIZE> words)
{
    if (sector >= NUM_SECTORS) {
        throw std::out_of_range("Requested sector does not exist");
    }

    sectors[sector] = words;
}
