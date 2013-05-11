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
    if (key.code >= sf::Keyboard::A && key.code <= sf::Keyboard::Z) {
        // sf::Keyboard::A should be 0 anyway, but do this just in case
        int letter = key.code - sf::Keyboard::A + 65; // 65 == 'A'
        if (!key.shift) {
            letter += 32; // 'a' - 'A'
        }
        return letter;
    } else if (key.code >= sf::Keyboard::Num0 && key.code <= sf::Keyboard::Num9 && !key.shift) {
        return key.code - sf::Keyboard::Num0 + 48; // 48 == '0'
    } else if (key.code >= sf::Keyboard::Numpad0 && key.code <= sf::Keyboard::Numpad9) {
        return key.code - sf::Keyboard::Numpad0 + 48; // 48 == '0'
    } else if (key.code == sf::Keyboard::LShift || key.code == sf::Keyboard::RShift) {
        return galaxy::saturn::keyboard::KEY_SHIFT;
    } else if (key.code == sf::Keyboard::LControl || key.code == sf::Keyboard::RControl) {
        return galaxy::saturn::keyboard::KEY_CONTROL;
    } else {
        switch (key.code) {
            case sf::Keyboard::SemiColon:
                if (key.shift)
                    return 58;
                else
                    return 59;
            case sf::Keyboard::Comma:
                if (key.shift)
                    return 60;
                else
                    return 44;
            case sf::Keyboard::Period:
                if (key.shift)
                    return 62;
                else
                    return 46;
            case sf::Keyboard::Quote:
                if (key.shift)
                    return 34;
                else
                    return 39;
            case sf::Keyboard::Slash:
                if (key.shift)
                    return 63;
                else
                    return 47;
            case sf::Keyboard::BackSlash:
                if (key.shift)
                    return 124;
                else
                    return 92;
            case sf::Keyboard::Tilde:
                if (key.shift)
                    return 126;
                else
                    return 96;
            case sf::Keyboard::Equal:
                if (key.shift)
                    return 43;
                else
                    return 61;
            case sf::Keyboard::Dash:
                if (key.shift)
                    return 95;
                else
                    return 45;
            case sf::Keyboard::Space:
                return 32;
            case sf::Keyboard::Return:
                return galaxy::saturn::keyboard::KEY_RETURN;
            case sf::Keyboard::BackSpace:
                return galaxy::saturn::keyboard::KEY_BACKSPACE;
            case sf::Keyboard::Add:
                return 43;
            case sf::Keyboard::Subtract:
                return 45;
            case sf::Keyboard::Multiply:
                return 42;
            case sf::Keyboard::Divide:
                return 47;
            case sf::Keyboard::Left:
                return galaxy::saturn::keyboard::KEY_ARROW_LEFT;
            case sf::Keyboard::Right:
                return galaxy::saturn::keyboard::KEY_ARROW_RIGHT;
            case sf::Keyboard::Up:
                return galaxy::saturn::keyboard::KEY_ARROW_UP;
            case sf::Keyboard::Down:
                return galaxy::saturn::keyboard::KEY_ARROW_DOWN;
            // all Nums are guaranteed to be with shift key by this point
            case sf::Keyboard::Num0:
                return 41;
            case sf::Keyboard::Num1:
                return 33;
            case sf::Keyboard::Num2:
                return 64;
            case sf::Keyboard::Num3:
                return 35;
            case sf::Keyboard::Num4:
                return 36;
            case sf::Keyboard::Num5:
                return 37;
            case sf::Keyboard::Num6:
                return 94;
            case sf::Keyboard::Num7:
                return 38;
            case sf::Keyboard::Num8:
                return 42;
            case sf::Keyboard::Num9:
                return 40;
        }
        return 0;
    }
}
