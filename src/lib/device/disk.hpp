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

#ifndef DISK_HPP
#define DISK_HPP

#include <libsaturn.hpp>
#include <cstdint>

namespace galaxy {
    namespace saturn {
        /**
         * represents a data disk
         */
        class disk {
            protected:
                bool wp;
            public:
                const static int NUM_SECTORS = 1440;
                const static int SECTOR_SIZE = 512;

                virtual ~disk() {}

                /// reads a sector from the disk
                virtual std::array<std::uint16_t, SECTOR_SIZE> read_sector(std::uint16_t sector) = 0;

                /// writes a sector to the disk
                virtual void write_sector(std::uint16_t sector, std::array<std::uint16_t, SECTOR_SIZE>) = 0;

                bool write_protected();
        };
    }
}

#endif
