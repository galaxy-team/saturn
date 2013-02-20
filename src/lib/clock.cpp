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
#include <iostream>

void galaxy::saturn::clock::interrupt()
{
    switch(cpu->A) {
        /**
         * The B register is read, and the clock will tick 60/B times per second.
         * If B is 0, the clock is turned off.
         */
        case 0:
            cycles_per_tick = (cpu->B * clock_speed) / 60;
            std::cout << "Cycles per tick set to 0x" << std::hex << cycles_per_tick << std::endl;
            ticks = 0;
            cycles = 0;
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
            std::cout << "Received interrupt change request 0x" << std::hex << cpu->B << std::endl;
            interrupt_message = cpu->B;
            std::cout << "Set interrupt_message to 0x" << std::hex << interrupt_message << std::endl;
    }
}

void galaxy::saturn::clock::cycle()
{
    if (cycles_per_tick != 0) {
        cycles++;
        if (cycles >= cycles_per_tick) {
            cycles = 0;
            ticks++;
            std::cout << "Interrupt message is 0x" << std::hex << interrupt_message << std::endl;
            if (interrupt_message != 0) {
                std::cout << "Triggering interrupt" << std::endl;
                cpu->interrupt(interrupt_message);
            }
        }
    }
}
