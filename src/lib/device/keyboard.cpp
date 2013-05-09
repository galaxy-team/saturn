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

void galaxy::saturn::keyboard::press(std::uint16_t key)
{
    int key_type = galaxy::saturn::keyboard::validate_key(key);
    if (key_type) {
        pressed.insert(key);
        if (key_type == 1) { // only non-alphanumeric keys interrupt on keypress
            if (interrupt_message)
                cpu->interrupt(interrupt_message);
        }
    }
}

void galaxy::saturn::keyboard::release(std::uint16_t key)
{
    int key_type = galaxy::saturn::keyboard::validate_key(key);
    if (key_type) {
        pressed.erase(key);
        if (key_type == 2) { // key is alphanumeric
            buffer.push_back(key); // key has been pressed and then released; put it in buffer
        }

        if (interrupt_message) // alphanumeric keys interrupt on type and non-alphanumeric keys itnerrupt on keyup
            cpu->interrupt(interrupt_message);
    }
}

int galaxy::saturn::keyboard::validate_key(std::uint16_t key)
{
    if (0x20 <= key && key <= 0x7f)
        return 2;

    switch (key) {
        case KEY_BACKSPACE:
        case KEY_ENTER:
        case KEY_INSERT:
        case KEY_DELETE:
        case KEY_ARROW_UP:
        case KEY_ARROW_DOWN:
        case KEY_ARROW_LEFT:
        case KEY_ARROW_RIGHT:
        case KEY_SHIFT:
        case KEY_CONTROL:
            return 1;
    }

    return 0;
}
