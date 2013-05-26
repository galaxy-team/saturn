/*

This file is part of libsaturn.

libsaturn is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libsaturn is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with libsaturn.  If not, see <http://www.gnu.org/licenses/>.

Your copy of the GNU Lesser General Public License should be in the
file named "LICENSE-LGPL.txt".

*/

#include <libsaturn.hpp>
#include <device.hpp>
#include <m35fd.hpp>

#include <cmath>

#define DEBUG(code) if (debug) {std::cout << code << std::endl;}


void galaxy::saturn::m35fd::interrupt()
{
    bool debug = true;
    switch(cpu->A) {
        /**
         * Poll device;
         * Sets B to the current state and C to the last error
         * since the last device poll.
         */
        case 0:
	    cpu->C = last_error_since_poll;
	    cpu->B = state();
	    break;

        /**
         * Set interupt;
         * Enables interrupts and sets the message to X if X is anything
         * other than 0, disables interrupts if X is 0. When interrupts are enabled,
         * the M35FD will trigger an interrupt on the DCPU-16 whenever the state or
         * error message changes.
         */
        case 1:
            // set interupt message to X
            interrupt_message = cpu->X;
            break;

        /**
         * Read sector
         * Reads sector X to DCPU ram starting at Y.
         * Sets B to 1 if reading is possible and has been started, anything else if it
         * fails. Reading is only possible if the state is STATE_READY or
         * STATE_READY_WP.
         * Protects against partial reads.
         */
        case 2:
            if (state() == STATE_READY || state() == STATE_READY_WP){
                try {
                    buffer = floppy_disk->read_sector(cpu->X);
                    cpu->B = 1;
                    read_to = cpu->Y;
                    reading = true;
                    cycles_left = get_delay_cycles(cpu->X);
                    current_sector = cpu->X;
                } catch (std::out_of_range& e) {
                    cpu->B = 0;
                }
            }

            break;

        /**
         * Write sector;
         * Writes sector X from DCPU ram starting at Y.
         * Sets B to 1 if writing is possible and has been started, anything else if it
         * fails. Writing is only possible if the state is STATE_READY.
         * Protects against partial writes.
         */
        case 3:
            if (state() == STATE_READY) {
                // temporary storage for the selected sector to write
                int read_from = cpu->Y;

                auto ram_begin = cpu->ram.begin(), ram_end = cpu->ram.begin();
                std::advance(ram_begin, read_from);
                std::advance(ram_end, read_from + SECTOR_SIZE);

                std::copy(
                    ram_begin,               // copy start
                    ram_end, // copy end
                    buffer.begin());                   // copy destination

                floppy_disk->write_sector(cpu->X, buffer);
                writing = true;
                cpu->B = 1;
                cycles_left = get_delay_cycles(cpu->X);
                current_sector = cpu->X;
            } else if (state() == STATE_READY_WP) {
                // the drive is set to be read only, error out
                last_error_since_poll = FD_ERROR_PROTECTED;
                cpu->B = 0;
            } else {
                cpu->B = 0;
            }
            break;
    }
}

void galaxy::saturn::m35fd::cycle() {
    if (reading || writing) {
        if (cycles_left > 0) {
            cycles_left--;
        } else {
            if (reading) {
                auto ram_begin = cpu->ram.begin();
                std::advance(ram_begin, read_to);
                std::cout << "Reading to RAM" << std::endl;
                std::copy(
                    buffer.begin(),               // copy start
                    buffer.end(),                 // copy end
                    ram_begin);                   // copy destination

                read_to = 0;
            }
            reading = false;
            writing = false;
        }
    }
}


int galaxy::saturn::m35fd::get_delay_cycles(int sector) {
    int tracks_seeked = std::abs((current_sector / SECTORS_PER_TRACK) - (sector / SECTORS_PER_TRACK));
    int track_seek_time = tracks_seeked * MILLISECONDS_PER_TRACK_SEEKED * (cpu->clock_speed / 1000);

    double rwtime = 512.0 / 1000.0 / 30.7;
    int rwdelay = (cpu->clock_speed / 1000) * rwtime;
    return track_seek_time + rwdelay;
}

void galaxy::saturn::m35fd::insert_disk(std::unique_ptr<galaxy::saturn::disk>& floppy_disk_ptr) {
    floppy_disk = std::move(floppy_disk_ptr);
    disk_loaded = true;
}

void galaxy::saturn::m35fd::insert_disk(galaxy::saturn::disk* floppy_disk_ptr) {
    floppy_disk = std::unique_ptr<disk>(floppy_disk_ptr);
    disk_loaded = true;
}

std::unique_ptr<galaxy::saturn::disk> galaxy::saturn::m35fd::eject_disk() {
    if (disk_loaded){
        std::unique_ptr<galaxy::saturn::disk> floppy_disk_ref = std::move(floppy_disk);
        floppy_disk = 0x0;
        disk_loaded = false;
        return std::move(floppy_disk_ref);
    } else {
        return 0;
    }
}

std::uint16_t galaxy::saturn::m35fd::state()
{
    if (reading || writing)
        return STATE_BUSY;
    if (!disk_loaded)
        return STATE_NO_MEDIA;
    if (floppy_disk->write_protected())
        return STATE_READY_WP;
    return STATE_READY;
}

bool galaxy::saturn::m35fd::get_last_error()
{
    return last_error_since_poll;
}
