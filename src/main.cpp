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
#include <lem1802.hpp>
#include <clock.hpp>
#include <keyboard.hpp>
#include <m35fd.hpp>
#include <invalid_opcode.hpp>
#include <queue_overflow.hpp>
//#include <utilities.hpp>

/* implementation specific */
#include "LEM1802Window.hpp"
#include "keyboard_adaptor.hpp"
#include <SFML/Graphics.hpp>

/* standard library */
#include <fstream>
#include <iostream>
#include <memory>

/* third party */
#include "OptionParser.h"


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
        .help("Provide a floppy disk image");

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
    std::basic_string<char> binary_filename = args[0];

    // grab the number of LEM1802's the user wants to have attached
    int num_lems = 1;
    if (options.get("num_lems") != ""){
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
    // FIX: use cpu.flash()
    for (int i = 0; i < (size / 2) && i < cpu.ram.size(); i++) {
        cpu.ram[i] = (buffer[i * 2]) << 0x8;
        cpu.ram[i] ^= buffer[i * 2 + 1] & 0xff;
    }

    delete[] buffer;


    // once i figure out how to read in multiple arguments, these code blocks will be re-written
    std::vector<std::string> floppy_disk_image_filenames;
    if (options["disk_image_filename"] != ""){
        std::cout << "Floppy disk detected; " << options["disk_image_filename"] << std::endl;
        floppy_disk_image_filenames.push_back(
             options["disk_image_filename"]);

    }

    std::basic_string<char> cur_disk_image_filename;
    int disk_image_filesize;

    for (std::vector<int>::size_type i = 0; i != floppy_disk_image_filenames.size(); i++){
     
        // we don't want to limit the number of floppy disks the user can attach, so we loop

        // create a new floppy drive, attach it to the cpu, and store a reference
        galaxy::saturn::m35fd& m35fd_ref = static_cast<galaxy::saturn::m35fd&>(cpu.attach_device(new galaxy::saturn::m35fd()));

        cur_disk_image_filename = floppy_disk_image_filenames[i];

        std::ifstream disk_image;
        // we should probably make sure the filename is absolute here
        disk_image.open(cur_disk_image_filename, std::ios::in | std::ios::binary);

        // ensure the file is actually open :P
        if ((disk_image && disk_image.good() && disk_image.is_open())) {
            disk_image.seekg(0, std::ios::end);
            disk_image_filesize = disk_image.tellg();

            std::cout << "Filesize obtained; " << disk_image_filesize << " bytes" << std::endl;

            // create an appropriately sized buffer and read into it
            char* buffer = new char[disk_image_filesize];
            disk_image.read(buffer, disk_image_filesize);

            m35fd_ref.read_in_image(buffer, disk_image_filesize);

            // wipe the buffer clean, close the file
            delete[] buffer;
        } else {
             std::cerr << "Error: could not open file \"" << cur_disk_image_filename << "\"" << std::endl;
             return -1;
        }
        disk_image.close();

        // if the file is read only, mark the floppy as such.
        // TODO: determine is this should be configurable via some other method
/*        if (file_utils.is_read_only(cur_disk_image_filename)) {
            m35fd_ref.is_read_only = true;
        } else {
            m35fd_ref.is_read_only = false;
        }*/
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

        // and compute however many cycles we must to keep in time
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
