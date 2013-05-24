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

/* libsaturn */
#include <libsaturn.hpp>
#include <queue_overflow.hpp>
#include <invalid_opcode.hpp>
#include <utilities.hpp>

/* libsaturn devices */
#include <clock.hpp>
#include <keyboard.hpp>
#include <m35fd.hpp>
#include <lem1802.hpp>
#include <fstream_disk.hpp>

/* implementation specific */
#include "LEM1802Window.hpp"
#include "keyboard_adaptor.hpp"

/* standard library */
#include <fstream>
#include <iostream>
#include <memory>

/* third party */
#include "OptionParser.h"
#include <SFML/Graphics.hpp>

void attach_m35fd(galaxy::saturn::dcpu& cpu, std::string filename){
    // create a new floppy drive, attach it to the cpu, and store a reference
    galaxy::saturn::m35fd& m35fd_ref = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

    m35fd_ref.insert_disk(std::unique_ptr<galaxy::saturn::fstream_disk>(new galaxy::saturn::fstream_disk(filename)));
}


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

    parser.add_option("-d", "--add-disk")
        .dest("disk_image_filename")
        .type("STRING")
        .action("append") // this is required if you want to allow
                          // the user to specify this more than once
        .help("Attach a floppy with a disk image loaded");

    parser.add_option("--add-ro-disk")
        .dest("disk_image_filename_ro")
        .type("STRING")
        .action("append") // this is required if you want to allow
                          // the user to specify this more than once
        .help("Attach a floppy with a disk image loaded, and set it as read only");

    // parse the buggers - Dom
    optparse::Values options = parser.parse_args(argc, argv);
    std::vector<std::string> args = parser.args();

    if (args.empty())
    {
        // if no positional (required) arguments were provided, print help and exit
        parser.print_help();
        return -1;
    }

    // the binary filename is the first of the positional arguments
    std::string binary_filename = args[0];

    // grab the number of LEM1802's the user wants to have attached
    int num_lems = 1;
    if (std::string(options.get("num_lems")) != ""){
         num_lems = (int)options.get("num_lems");
    }

    // read in the binary file, create the dcpu instance, and flash it with the binary
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
    // TODO: FIX: use cpu.flash()
    for (int i = 0; i < (size / 2) && i < cpu.ram.size(); i++) {
        cpu.ram[i] = (buffer[i * 2]) << 0x8;
        cpu.ram[i] ^= buffer[i * 2 + 1] & 0xff;
    }

    delete[] buffer;

    // setup the floppy disks
    if (options.all("disk_image_filename").size() != 0 || options.all("disk_image_filename_ro").size() != 0){
        // we start by grabbing a list of floppy names, and tell the user how many we are loading
        std::list<std::string> normal_filenames = options.all("disk_image_filename");
        std::list<std::string> ro_filenames     = options.all("disk_image_filename_ro");
        std::cout << "Loading " << normal_filenames.size() + ro_filenames.size() << " floppy disks" << std::endl;

        // thence we iterate through, using my handy helper function attach_m35fd. normal first
        for (std::list<std::string>::iterator i = normal_filenames.begin(); i != normal_filenames.end(); i++) {
            attach_m35fd(cpu, *i);
        }

        // thence the read only disks
        for (std::list<std::string>::iterator i = ro_filenames.begin(); i != ro_filenames.end(); i++) {
            attach_m35fd(cpu, *i);
        }
    }

    // create the LEM1802 windows
    std::vector<std::unique_ptr<LEM1802Window>> lem_windows;
    for (int i = 0; i < num_lems; i++) {
        std::unique_ptr<LEM1802Window> win (new LEM1802Window(static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()))));
        lem_windows.push_back(std::move(win));
    }
 
    // attach the clock
    cpu.attach_device(new galaxy::saturn::clock());
  
    // attack the keyboard
    keyboard_adaptor keyboard (static_cast<galaxy::saturn::keyboard&>(cpu.attach_device(new galaxy::saturn::keyboard())));

    // initialise the timing clock
    sf::Clock clock;


    bool running = true;
 
    // start the main loop
    while (running)
    {
        // we check for events on each window
        for (auto it = lem_windows.begin(); it != lem_windows.end(); ++it) {
            sf::Event event;
            while ((*it)->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    running = false;
                else if (event.type == sf::Event::TextEntered)
                    keyboard.key_type(event.text);
                else if (event.type == sf::Event::KeyPressed)
                    keyboard.key_press(event.key);
                else if (event.type == sf::Event::KeyReleased)
                    keyboard.key_release(event.key);
            }
        }

        // and compute however many cycles we must perform to keep in time
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

        // update all the windows with their appropriate contents
        for (auto it = lem_windows.begin(); it != lem_windows.end(); ++it)
            (*it)->update();

    }

    return 0;
}
