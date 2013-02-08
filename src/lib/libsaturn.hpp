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

#include <cstdint>
#include <vector>
#include <queue>
#include <memory>
#include <array>
#include "device.hpp"

namespace galaxy {
    namespace saturn {
        /**
         * a dcpu represents a virtual DCPU machine
         */
        class dcpu {
        protected:
            bool interrupt_queue_enabled;
            bool queue_interrupts;
            std::vector<std::unique_ptr<device>> devices;
            std::queue<uint16_t> interrupt_queue;

            /// parse the given value and return it
            std::uint16_t get_value(std::uint16_t);

            /**
             * parse the given address and set it
             * to the given value
             */
            void set_value(std::uint16_t, std::uint16_t);

        public:
            std::array<std::uint16_t, 0x10000> ram;
            uint16_t A, B, C, X, Y, Z, I, J, PC, SP, EX, IA;

            /**
             * The number of cycles the DCPU should sleep.
             * Incremented whenever an instruction takes
             * more than one cycle, and decremented when
             * the DCPU sleeps for a cycle
             */
            int sleep_cycles;

            /// initialize the CPU to default values
            dcpu()  :   interrupt_queue_enabled(false), queue_interrupts(false),
                        A(0), B(0), C(0), X(0), Y(0), Z(0), I(0), J(0),
                        PC(0), SP(0), EX(0), IA(0), 
                        sleep_cycles(0) {}

            /// perform a CPU cycle
            void cycle();

            /**
             * Trigger a software interrupt with the given message.
             */
            void interrupt(std::uint16_t);

            /// attach a hardware device to the CPU. steals the unique_ptr, and so returns a reference so you can still use it after attaching it.
            device& attach_device(std::unique_ptr<device>);

        /* THE ONE WHO IMPLEMENTS THIS CORRECTLY SHALL BE KING OF NAFSU
           FOR 3.0123 MILLISECONDS
            /// flash memory with iterators for a list of uint16_t
            template <typename Iter>
            void flash(Iter begin, Iter end);
         */

            /// flash memory with std::vector<std::uint16_t>
            void flash(
                std::vector<std::uint16_t>::const_iterator begin,
                std::vector<std::uint16_t>::const_iterator end
            );

            /// flash memory with std::<std::uint16_t, 0x10000>
            void flash(
                std::array<std::uint16_t, 0x10000>::const_iterator begin,
                std::array<std::uint16_t, 0x10000>::const_iterator end
            );

            /// Reset memory and registers
            void reset();

        };
    }
}

#endif
