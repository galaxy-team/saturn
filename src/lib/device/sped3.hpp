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

#ifndef SPED3_HPP
#define SPED3_HPP

#include <libsaturn.hpp>
#include <device.hpp>

#include <cstdint>

#include <vector>

namespace galaxy {
    namespace saturn {
        class device;

        /**
         * represents a 3D vertex
         * TODO: move this into its own file
         */
        struct vertex {
            enum colors {
                COLOR_BLACK,
                COLOR_RED,
                COLOR_GREEN,
                COLOR_BLUE
            };

            std::uint8_t x;
            std::uint8_t y;
            std::uint8_t z;

            int color;
            bool intense;
        };
        /**
         * represents a Mackapar Suspended Particle Exciter Display
         */
        class sped3 : public device {
            protected:
                std::uint16_t memory_map_offset;
                std::uint16_t num_vertices;

                std::uint16_t target_rotation;
                double current_rotation;

                std::uint16_t state;
                std::uint16_t error;
            public:
                /// initialize the device to values specified by the spec
                sped3() : device(0x42babf3c, 0x1eb37e91, 0x0003, "Mackapar Suspended Particle Exciter Display, Rev 3 (SPED-3)"),
                          memory_map_offset(0), num_vertices(0), target_rotation(0), current_rotation(0),
                          state(STATE_NO_DATA), error(ERROR_NONE) {}

                virtual void interrupt();
                virtual void cycle();

                /// return the vertices representing the model in the order to be drawn
                std::vector<vertex> vertices();

                /// return the SPED's rotation, in degrees
                double rotation();

                enum states {
                    STATE_NO_DATA,
                    STATE_RUNNING,
                    STATE_TURNING
                };

                /// There's been no error since the last poll.
                static const std::uint16_t ERROR_NONE;

                /// There's been some major software or hardware problem, try turning off and turning on the device again.
                static const std::uint16_t ERROR_BROKEN;

                /// The rotation speed of the SPED-3, in degrees per second
                static const double ROTATION_SPEED;
        };
    }
}

#endif
