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
#include <deque>
#include <set>

namespace galaxy {
    namespace saturn {
        class device;
        /**
         * represents the keyboard
         */
        class keyboard : public device{
            public:
                /// initialize the device to values specified by the spec
                keyboard() : device(0x30cf7406, 0x0, 0x1, "Generic Keyboard (compatible)"), interrupt_message(0) {}

                virtual void interrupt();

                /**
                 * Call when a key has been pressed down
                 * The argument must be a valid Generic Keyboard key, as defined below.
                 */
                void press(std::uint16_t key);

                /**
                 * Call when a key has been released
                 * The argument must be a valid Generic Keyboard key, as defined below.
                 */
                void release(std::uint16_t key);

                /// These are the ONLY keys valid for a Generic Keyboard
                static const std::uint16_t KEY_BACKSPACE = 0x10;
                static const std::uint16_t KEY_ENTER = 0x11;
                static const std::uint16_t KEY_INSERT = 0x12;
                static const std::uint16_t KEY_DELETE = 0x13;
                /// 0x20-0x7f: ASCII characters
                static const std::uint16_t KEY_ARROW_UP = 0x80;
                static const std::uint16_t KEY_ARROW_DOWN = 0x81;
                static const std::uint16_t KEY_ARROW_LEFT = 0x82;
                static const std::uint16_t KEY_ARROW_RIGHT = 0x83;
                static const std::uint16_t KEY_SHIFT = 0x90;
                static const std::uint16_t KEY_CONTROL = 0x91;

            private:
                std::deque<std::uint16_t> buffer;
                std::set<std::uint16_t> pressed;
                std::uint16_t interrupt_message;

                /**
                 * If key is alphanumeric, return 2
                 * Otherwise, if the key is still valid, return 1
                 * If the key isn't valid, return 0
                 */
                int validate_key(std::uint16_t key);
        };
    }
}

#endif
