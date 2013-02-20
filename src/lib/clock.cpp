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
#include <device.hpp>
#include <clock.hpp>

void galaxy::saturn::clock::interrupt()
{
    switch(cpu->A) {
        /**
         * The B register is read, and the clock will tick 60/B times per second.
         * If B is 0, the clock is turned off.
         */
        case 0:
            period = cpu->B;
            ticks = 0;
            break;

        /**
         * Store number of ticks elapsed since last call to 0 in C register
         */
        case 1:
            cpu->C = ticks;
            break;

        /**
         * If register B is non-zero, turn on interrupts with message B. If B is zero,
         * disable interrupts
         */
        case 2:
            if (cpu->B != 0) {
                interrupt_message = cpu->B;

            } else {
                interrupt_message = 0x0;
            }
    }
}

void galaxy::saturn::clock::cycle()
{
    if (cycles++ >= ((period / 60) / (1 / clock_speed))) {
        cycles = 0;
        ticks++;
        if (interrupt_message != 0) {
            cpu->interrupt(interrupt_message);
        }
    }
}
