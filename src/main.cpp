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
#include <clock.hpp>
#include <keyboard.hpp>
#include <invalid_opcode.hpp>
#include <queue_overflow.hpp>

#include "LEM1802Window.hpp"
#include "keyboard_adaptor.hpp"
#include <SFML/Graphics.hpp>
#include "OptionParser.h"

#include <fstream>
#include <iostream>
#include <memory>

int main(int argc, char** argv)
{
    // setup the command line argument parser
    optparse::OptionParser parser = optparse::OptionParser()
        .description("Saturn, Galaxy's emulator")
        .usage("usage: %prog [options] <binary>");

    parser.add_option("-n", "--num_lems")
        .dest("num_lems")
        .type("int")
        .help("Specify number of attached LEM1802's");

    // parse the buggers - Dom
    optparse::Values options = parser.parse_args(argc, argv);
    std::vector<std::string> args = parser.args();

    if (args.empty())
    {
        // if no positional (required) arguments were provided, print help and exit
        parser.print_help();
        return -1;
    }

    std::basic_string<char> binary_filename = args[0];

    int num_lems = 1;
    if ((int)options.get("num_lems") != 0){
         num_lems = (int)options.get("num_lems");
    }


    std::ifstream file;
    file.open(binary_filename, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: could not open file \"" << binary_filename << "\"" << std::endl;
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


    std::vector<std::unique_ptr<LEM1802Window>> lem_windows;
    for (int i = 0; i < num_lems; i++) {
        std::unique_ptr<LEM1802Window> win (new LEM1802Window(static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()))));
        lem_windows.push_back(std::move(win));
    }

    cpu.attach_device(new galaxy::saturn::clock());
    keyboard_adaptor keyboard (static_cast<galaxy::saturn::keyboard&>(cpu.attach_device(new galaxy::saturn::keyboard())));

    sf::Clock clock;


    bool running = true;

    while (running)
    {
        for (auto it = lem_windows.begin(); it != lem_windows.end(); ++it) {
            sf::Event event;
            while ((*it)->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    running = false;
                else if (event.type == sf::Event::KeyPressed)
                    keyboard.key_press(event.key);
                else if (event.type == sf::Event::KeyReleased)
                    keyboard.key_release(event.key);
            }
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

        for (auto it = lem_windows.begin(); it != lem_windows.end(); ++it)
            (*it)->update();

    }

    return 0;
}
