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

#include <lem1802.hpp>

#include <SFML/Graphics.hpp>

class LEM1802Window : public sf::RenderWindow {
    public:
        LEM1802Window(galaxy::saturn::lem1802& lem) : RenderWindow(sf::VideoMode((galaxy::saturn::lem1802::width + border * 2) * 4, (galaxy::saturn::lem1802::height + border * 2) * 4), "Saturn"), lem(lem)
        {
            screen_image.create(galaxy::saturn::lem1802::width, galaxy::saturn::lem1802::height, sf::Color(0, 0, 255));
            screen_texture.loadFromImage(screen_image);
            screen.setTexture(screen_texture);
            screen.setScale(sf::Vector2f(4.f, 4.f));
            screen.setPosition(sf::Vector2f(border * 4, border * 4));

            setFramerateLimit(30);
        }
        void update();
    private:
        galaxy::saturn::lem1802& lem;
        sf::Image screen_image;
        sf::Texture screen_texture;
        sf::Sprite screen;

        static const unsigned int border = 3;
};
