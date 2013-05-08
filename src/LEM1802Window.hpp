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
        LEM1802Window(galaxy::saturn::lem1802& lem) : super(sf::VideoMode(512, 384), "Saturn"), lem(lem)
        {
            screen_image.create(128, 96, sf::Color(0, 0, 255));
            screen_texture.loadFromImage(screen_image);
            screen.setTexture(screen_texture);
            screen.setScale(sf::Vector2f(4.f, 4.f));
        }
        void update();
    private:
        galaxy::saturn::lem1802& lem;
        sf::Image screen_image;
        sf::Texture screen_texture;
        sf::Sprite screen;
}
