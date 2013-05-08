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

#include "LEM1802Window.hpp"

void LEM1802Window::update()
{
    std::array<std::array<galaxy::saturn::pixel, 128>, 96> image = lem.image();

    sf::Uint8* pixels = new sf::Uint8[128 * 96 * 4];

    for (int y = 0; y < image.size(); y++) {
        for (int x = 0; x < image[y].size(); x++) {
            pixels[(y * 128 + x) * 4] = image[y][x].r;
            pixels[(y * 128 + x) * 4 + 1] = image[y][x].g;
            pixels[(y * 128 + x) * 4 + 2] = image[y][x].b;
            pixels[(y * 128 + x) * 4 + 3] = 255;
        }
    }

    screen_texture.update(pixels);

    clear();
    draw(screen);
    display();

    delete[] pixels;
}
