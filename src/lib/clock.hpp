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

#include <device.hpp>
#include <cstdint>

namespace galaxy {
    namespace saturn {
        /**
         * represents the hardware clock
         */
        class clock : device{
            protected:
                /// the clock ticks at 60/period times per second
                std::uint16_t period;

                /// stores the number of ticks since the period was set
                std::uint16_t ticks;

                /// if this is nonzero, an interrupt with this message will be triggered on each tick
                std::uint16_t interrupt_message;

                /// stores the number of cycles since last tick
                int cycles;

            public:
                /// clock speed of the dcpu (perhaps this should be protected and constant? depends on if the client should be able to change it)
                int clock_speed;

                /// initialize the device to values specified by the spec
                clock() : device(0x12d0b402, 0x0, 0x1, "Generic Clock (compatible)"), period(0x0), ticks(0x1),
                    interrupt_message(0x0), cycles(0), clock_speed(100000) {}

                virtual void interrupt();
                virtual void cycle();
        };
    }
}

#endif
