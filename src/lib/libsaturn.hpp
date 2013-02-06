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

#ifndef LIBSATURN_HPP
#define LIBSATURN_HPP

#include "device.hpp"

namespace galaxy {
    namespace saturn {
        /**
         * a dcpu represents a virtual DCPU machine
         */
        class dcpu {
            std::array<uint16_t, 0x10000> ram;
            uint16_t A, B, C, X, Y, Z, I, J, PC, SP, EX, IA;
            std::vector<device> devices;
            std::queue<uint16_t> interrrupt_queue;

            bool interrupts_enabled;
            bool on_fire;

            int clock_speed;

            // total cycle count
            // NOTE: not sure if this should be a larger type
            int total_cycles;

            // number of cycles left to execute;
            int cycles;

            /**
             * execute the indicated number of cycles
             */
            void execute(int num_cycles);

            /**
             * execute necessary number of cycles
             * to remain at clock speed
             */
            void execute();

            /**
             * attach a hardware device to the CPU
             */
            void attach_device(device hw);

            /**
             * flash memory with a std::vector<uint16_t>
             */
            void flash(
                std::vector::const_iterator begin,
                std::vector::const_iterator end
            );

            /**
             * Reset memory and registers
             */
            void reset();

        }
    }
}

#endif
