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

#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <libsaturn.hpp>
#include <device.hpp>
#include <cstdint>

namespace galaxy {
    namespace saturn {
        class device;
        /**
         * represents the keyboard
         */
        class keyboard : public device{
            public:
                /// initialize the device to values specified by the spec
                keyboard() : device(0x30cf7406, 0x0, 0x1, "Generic Keyboard (compatible)") {}

                virtual void interrupt();
                virtual void cycle();

                static const std::uint16_t backspace = 0x10;
                static const std::uint16_t enter = 0x11;
                static const std::uint16_t insert = 0x12;
                static const std::uint16_t delete = 0x13;
                static const std::uint16_t arrow_up = 0x80;
                static const std::uint16_t arrow_down = 0x81;
                static const std::uint16_t arrow_left = 0x82;
                static const std::uint16_t arrow_right = 0x83;
                static const std::uint16_t shift = 0x90;
                static const std::uint16_t control = 0x91;
        };
    }
}

#endif
