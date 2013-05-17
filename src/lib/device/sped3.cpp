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

#include <cmath>

const std::uint16_t galaxy::saturn::sped3::ERROR_NONE = 0x0000;
const std::uint16_t galaxy::saturn::sped3::ERROR_BROKEN = 0xFFFF;

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
    if (state == STATE_TURNING) {
        double displacement = current_rotation - target_rotation;
        double rotate_by = ROTATION_SPEED / cpu->clock_speed;

        if (std::abs(displacement) < rotate_by) {
            current_rotation = target_rotation;
            state = STATE_RUNNING;
        } else {

            if (displacement < -180)
                current_rotation -= rotate_by;
            else if (displacement < 0)
                current_rotation += rotate_by;
            else if (displacement > 180)
                current_rotation += rotate_by;
            else if (displacement > 0)
                current_rotation -= rotate_by;

            current_rotation = std::fmod(current_rotation, 360);
        }
    }
}

std::vector<galaxy::saturn::vertex> galaxy::saturn::sped3::model()
{
    std::vector<galaxy::saturn::vertex> model;

    for (std::uint16_t i = memory_map_offset; i < (static_cast<std::uint32_t>(memory_map_offset) + num_vertices * 2) % 0xffff; i += 2) {
        galaxy::saturn::vertex v;
        v.x = cpu->ram[i] & 0xff;
        v.y = (cpu->ram[i] >> 0xf) & 0xff;
        v.z = cpu->ram[(i + 1) % 0xffff] & 0xff;
        v.color = (cpu->ram[(i + 1) % 0xffff] >> 0xf) & 0x3;
        v.intense = (cpu->ram[(i + 1) % 0xffff] >> 0x11) & 0x1;

        model.push_back(v);
    }

    return model;
}
