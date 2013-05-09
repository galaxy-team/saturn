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
#include <keyboard.hpp>

void galaxy::saturn::keyboard::interrupt()
{
    switch(cpu->A) {
        /**
         * Clear keyboard buffer
         */
        case 0:
            buffer.clear();
            break;
        /**
         * Store next key typed in C register, or 0 if the buffer is empty
         */
        case 1:
            cpu->C = buffer.front();
            buffer.pop_front();
            break;

        /**
         * Set C register to 1 if the key specified by the B register is pressed, or
         * 0 if it's not pressed
         */
        case 2:
            if (pressed.find(cpu->B) != pressed.end())
                cpu->C = 1;
            else
                cpu->C = 0;
            break;
        /**
         * If register B is non-zero, turn on interrupts with message B. If B is zero,
         * disable interrupts
         */
        case 3:
            interrupt_message = cpu->B;
    }
}
