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

#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <libsaturn.hpp>
#include <device.hpp>
#include <cstdint>

namespace galaxy {
    namespace saturn {
        class device;
        /**
         * represents a Mackapar Suspended Particle Exciter Display
         */
        class sped3 : public device{
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

                /// No vertices queued up, device is in stand-by
                static const std::uint16_t STATE_NO_DATA = 0x0000;

                /// The device is projecting lines
                static const std::uint16_t STATE_RUNNING = 0x0001;

                /// The device is projecting lines and turning
                static const std::uint16_t STATE_TURNING = 0x0002;

                /// There's been no error since the last poll.
                static const std::uint16_t ERROR_NONE = 0x0000;

                /// There's been some major software or hardware problem, try turning off and turning on the device again.
                static const std::uint16_t ERROR_BROKEN = 0xffff;

                /// The rotation speed of the SPED-3, in degrees per second
                static constexpr double ROTATION_SPEED = 50;
        };
    }
}

#endif
