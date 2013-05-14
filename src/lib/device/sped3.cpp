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
#include <sped3.hpp>

void galaxy::saturn::sped3::interrupt()
{
    switch(cpu->A) {
        /**
         * Poll device. Sets B to the current state (see below) and C to the last error
         * since the last device poll.
         */
        case 0:
            cpu->B = state;
            cpu->C = error;

            error = ERROR_NONE;
            break;

        /**
         * Map region. Sets the memory map offset to X, and the total number of vertices
         * to render to Y. See below for the encoding information.
         */
        case 1:
            memory_map_offset = cpu->X;
            num_vertices = cpu->Y;

            if (num_vertices == 0)
                state = STATE_NO_DATA;
            else if (state == STATE_NO_DATA)
                state = STATE_RUNNING;

            break;

        /**
         * Rotate device. Sets the target rotation for the device to X%360 degrees.
         */
        case 2:
            target_rotation = cpu->X % 360;
            state = STATE_TURNING;
            break;
    }
}

void galaxy::saturn::sped3::cycle()
{
}
