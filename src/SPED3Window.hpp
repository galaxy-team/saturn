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
#include <SFML/OpenGL.hpp>

class SPED3Window : public sf::Window {
    public:
        SPED3Window(galaxy::saturn::sped3& sped) : Window(sf::VideoMode(512, 512), "Saturn"), sped(sped)
        {
            setVerticalSyncEnabled(true);
        }
        void update();
    private:
        galaxy::saturn::sped3& sped;
};
