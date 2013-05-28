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

#include <libsaturn.hpp>

#include <SFML/Window.hpp>

class keyboard_adaptor {
    public:
        keyboard_adaptor(galaxy::saturn::keyboard& keyboard) : keyboard(keyboard) {}
        void key_press(sf::Event::KeyEvent event);
        void key_release(sf::Event::KeyEvent event);

        /// this function instantaneously presses and releases the key; try to improve this if possible
        void key_type(sf::Event::TextEvent event);
    private:
        // returns a DCPU key code if valid, 0 otherwise
        std::uint16_t event_to_dcpu(sf::Event::KeyEvent event);
        galaxy::saturn::keyboard& keyboard;
};
