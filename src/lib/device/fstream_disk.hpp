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

#ifndef FSTREAM_DISK_HPP
#define FSTREAM_DISK_HPP

#include <libsaturn.hpp>
#include <disk.hpp>
#include <cstdint>

#include <fstream>

namespace galaxy {
    namespace saturn {
        /**
         * represents a data disk, stored in a file
         */
        class fstream_disk : public disk{
            protected:
                std::fstream file;
            public:
                /// initialize the device to values specified by the spec
                fstream_disk(std::string filename)
                {
                    wp = false;
                    file.open(filename, std::ios::out | std::ios::in | std::ios::binary | std::ios::ate);

                    if (!file.is_open()) {
                        // couldn't open file, try creating it
                        file.open(filename, std::ios::out | std::ios::in | std::ios::binary | std::ios::ate | std::ios::trunc);
                    }

                    if (!file.is_open()) {
                        // still couldn't open file, try opening it in read-only mode
                        file.open(filename, std::ios::in | std::ios::binary | std::ios::ate | std::ios::trunc);
                        wp = true;
                    }

                    int s = file.tellg();
                    if (s < NUM_SECTORS * SECTOR_SIZE) {
                        while (file.tellg() < NUM_SECTORS * SECTOR_SIZE) {
                            file.put(0);
                        }
                    }
                }

                virtual ~fstream_disk()
                {
                    file.close();
                }

                virtual std::array<std::uint16_t, SECTOR_SIZE> read_sector(std::uint16_t sector);
                virtual void write_sector(std::uint16_t sector, std::array<std::uint16_t, SECTOR_SIZE>);
        };
    }
}

#endif
