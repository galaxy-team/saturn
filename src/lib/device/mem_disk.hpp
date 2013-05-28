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

#ifndef MEM_DISK_HPP
#define MEM_DISK_HPP

#include <libsaturn.hpp>
#include <disk.hpp>
#include <cstdint>

#include <array>

namespace galaxy {
    namespace saturn {
        /**
         * represents a data disk, stored in a file
         */
        class mem_disk : public disk{
            protected:
                std::array<std::array<std::uint16_t, SECTOR_SIZE>, NUM_SECTORS> sectors;
            public:

                virtual std::array<std::uint16_t, SECTOR_SIZE> read_sector(std::uint16_t sector);
                virtual void write_sector(std::uint16_t sector, std::array<std::uint16_t, SECTOR_SIZE>);
        };
    }
}

#endif
