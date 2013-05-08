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
#include <lem1802.hpp>
#include <invalid_opcode.hpp>
#include <queue_overflow.hpp>

#include "LEM1802Window.hpp"
#include <SFML/Graphics.hpp>

#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
    // temporary parameter parsing
    if (argc != 2) {
        std::cerr << "Error: Invalid usage. Usage is `saturn <binary>` or somesuch" << std::endl;
        return -1;
    }

    std::ifstream file;
    file.open(argv[1], std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: could not open file \"" << argv[1] << "\"" << std::endl;
        return -1;
    }

    int size = file.tellg();
    char* buffer = new char[size];
    file.seekg(0, std::ios::beg);
    file.read(buffer, size);
    file.close();

    galaxy::saturn::dcpu cpu;
    // FIX: use cpu.flash()
    for (int i = 0; i < (size / 2) && i < cpu.ram.size(); i++) {
        cpu.ram[i] = (buffer[i * 2]) << 0x8;
        cpu.ram[i] ^= buffer[i * 2 + 1] & 0xff;
    }

    delete[] buffer;

    galaxy::saturn::lem1802& lem = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    LEM1802Window lem_window(lem);

    sf::Clock clock;


    bool running = true;

    while (running)
    {
        sf::Event event;
        while (lem_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                running = false;
        }

        try {
            sf::Int32 msec = clock.getElapsedTime().asMilliseconds();
            clock.restart();
            int cycles = (cpu.clock_speed / 1000) * msec;
            //std::cout << "Executing " << std::dec << cycles << " cycles." << std::endl;
            while (cycles > 0) {
                cpu.cycle();
                cycles--;
            }
        } catch(galaxy::saturn::invalid_opcode& e) {
            std::cerr << "Error: invalid opcode: 0x" << std::hex << cpu.ram[cpu.PC] << " at 0x" << std::hex << cpu.PC << std::endl;
            running = false;
        }

        lem_window.update();

    }

    return 0;
}
