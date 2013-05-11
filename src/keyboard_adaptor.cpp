/*

This file is part of saturn.

saturn is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

saturn is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with saturn.  If not, see <http://www.gnu.org/licenses/>.

Your copy of the GNU General Public License should be in the
file named "LICENSE.txt".

*/

#include "keyboard_adaptor.hpp"

void keyboard_adaptor::key_press(sf::Event::KeyEvent event)
{
    keyboard.press(event_to_dcpu(event));
}

void keyboard_adaptor::key_release(sf::Event::KeyEvent event)
{
    keyboard.release(event_to_dcpu(event));
}

void keyboard_adaptor::key_type(sf::Event::TextEvent event)
{
    if (event.unicode < 0x7f && event.unicode >= 0x20) {
        keyboard.press(event.unicode);
        keyboard.release(event.unicode);
    }
}

std::uint16_t keyboard_adaptor::event_to_dcpu(sf::Event::KeyEvent key)
{
    switch (key.code) {
        case sf::Keyboard::Return:
            return galaxy::saturn::keyboard::KEY_RETURN;
        case sf::Keyboard::BackSpace:
            return galaxy::saturn::keyboard::KEY_BACKSPACE;
        case sf::Keyboard::Left:
            return galaxy::saturn::keyboard::KEY_ARROW_LEFT;
        case sf::Keyboard::Right:
            return galaxy::saturn::keyboard::KEY_ARROW_RIGHT;
        case sf::Keyboard::Up:
            return galaxy::saturn::keyboard::KEY_ARROW_UP;
        case sf::Keyboard::Down:
            return galaxy::saturn::keyboard::KEY_ARROW_DOWN;
    }
    return 0;
}
