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
//	    DEBUG("Last error since poll; " << last_error_since_poll << ", current_state; " << current_state);
	    cpu->C = last_error_since_poll;
	    cpu->B = current_state;
	    break;

        /**
         * Set interupt;
         * Enables interrupts and sets the message to X if X is anything
         * other than 0, disables interrupts if X is 0. When interrupts are enabled,
         * the M35FD will trigger an interrupt on the DCPU-16 whenever the state or
         * error message changes.
         */
        case 1:
//          DEBUG("Setting interrupt message to; 0x" << std::hex << cpu->X);
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
            if (current_state == STATE_READY || current_state == STATE_READY_WP){
//                DEBUG("Reading")
                try {
                    std::array<uint16_t, SECTOR_SIZE> sector = floppy_disk->read_sector(cpu->X);
                    cpu->B = 1;
                    //DEBUG("Reading failed")
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
            if (current_state == STATE_READY) {
                if (floppy_disk->write_protected()) {
                    // the drive is set to be read only, error out
                    last_error_since_poll = FD_ERROR_PROTECTED;
                    cpu->B = 0;
                } else {
                    std::array<uint16_t, 512> blah;
                    floppy_disk->write_sector(cpu->X, blah);
                }
            } else {
                cpu->B = 0;
            }
//            if (cpu->B == 0) DEBUG("Writing failed")
            break;
    }
}

void galaxy::saturn::m35fd::cycle() {
}


int galaxy::saturn::m35fd::get_track_seek_time(int current_track, int sector) {
    int tracks_seeked = (current_track / SECTORS_PER_TRACK) - (sector / SECTORS_PER_TRACK);

    // ensure it aint negitive; better way to do this?
    if (!tracks_seeked >= 0) {
	tracks_seeked = 0 - tracks_seeked;
    }

    int track_seek_time = tracks_seeked * MILLISECONDS_PER_TRACK_SEEKED;
    return track_seek_time;
}

// TODO: implement these two
void galaxy::saturn::m35fd::insert_disk(std::unique_ptr<galaxy::saturn::disk> floppy_disk_ptr) {
    floppy_disk = std::move(floppy_disk_ptr);
    disk_loaded = true;
}

std::unique_ptr<galaxy::saturn::disk> galaxy::saturn::m35fd::eject_disk() {
    std::unique_ptr<galaxy::saturn::disk> ref = std::move(floppy_disk);
    floppy_disk = 0x0;
    disk_loaded = false;
    return ref;
}

