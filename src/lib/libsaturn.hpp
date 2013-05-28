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

#include <device.hpp>

#include <cstdint>
#include <vector>
#include <queue>
#include <memory>
#include <array>

namespace galaxy {
    namespace saturn {
        /**
         * a dcpu represents a virtual DCPU machine
         */
        class dcpu {
        protected:
            bool queue_interrupts;

            /// if guard_interrupts is true, a cycle is executing and interrupts should not be triggered until the next cycle
            bool guard_interrupts;
            std::vector<std::unique_ptr<device>> devices;
            std::queue<uint16_t> interrupt_queue;

            std::uint16_t literal_value;

            /**
             * parse the given value and return a reference to it
             * if the second argument is true, it will treat the
             * value as being the "a" value in the instruction;
             * otherwise, it will treat it as being the "b" value
             */
            std::uint16_t& get_reference(std::uint16_t, bool);

        public:
            const static int RAM_SIZE = 0x10000;
            std::array<std::uint16_t, RAM_SIZE> ram;
            uint16_t A, B, C, X, Y, Z, I, J, PC, SP, EX, IA;

            /**
             * The number of cycles the DCPU should sleep.
             * Incremented whenever an instruction takes
             * more than one cycle, and decremented when
             * the DCPU sleeps for a cycle
             */
            int sleep_cycles;

            /// clock speed of the dcpu
            int clock_speed;

            /// initialize the CPU to default values
            dcpu()  :   queue_interrupts(false), guard_interrupts(false),
                        A(0), B(0), C(0), X(0), Y(0), Z(0), I(0), J(0),
                        PC(0), SP(0), EX(0), IA(0),
                        sleep_cycles(0), clock_speed(100000)
            {
                ram.fill(0);
            }

            /// perform a CPU cycle
            void cycle();

            /**
             * Trigger a software interrupt with the given message.
             */
            void interrupt(std::uint16_t);

            /// attach a hardware device to the CPU. converts the pointer to a unique_ptr, and so returns a reference so you can still use it after attaching it.
            device& attach_device(device*);

            template<class Iterator, class=
            typename std::enable_if<std::is_same<typename std::iterator_traits<Iterator>::value_type, uint16_t>::value>::type>
            void flash(Iterator begin, Iterator end)
            {
                std::copy(begin, end, ram.begin());
            }

            /// Reset memory and registers
            void reset();

            /// return queue_interrupts
            bool interrupt_queue_enabled();

            /// indicated whether or not execution has begun
            bool execution_has_begun = false;

            /// fairly self explanatory
            class device_modification_after_cycle_error : public std::exception {
                public:
                    device_modification_after_cycle_error(const std::string& message) : std::exception() {
                        const char* what() throw();
                    };
            };
            const std::vector<std::unique_ptr<galaxy::saturn::device>> &get_devices() const;
        };
    }
}

#endif
