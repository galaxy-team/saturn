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


void galaxy::saturn::m35fd::interrupt()
{
    switch(cpu->A) {
        /**
         * Poll device;
         * Sets B to the current state and C to the last error
         * since the last device poll.
         */
        case 0:
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
            // this will work, right?
            if (cpu->X != 0) {
                // set interupt message to X
                interrupt_message = cpu->X;
            } else {
                // disable interupts
                interrupt_message = 0;
            }
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
                int sector = cpu->X;
                int read_to = cpu->Y;

                if (0 <= sector && sector <= SECTOR_NUM) {
                    // ensure the user is not trying to read from outside the floppy disk image
                    cpu->B = 0;
                } else if (0 <= read_to && read_to <= cpu->RAM_SIZE) {
                    // ensure the user is not trying to read from outside the ram
                    cpu->B = 0;
                } else {
                    // if everything seems to be in order...

                    // okay, 2.4ms per track seeked you say?
                    int tracks_seeked = (current_track / SECTORS_PER_TRACK) - (sector / SECTORS_PER_TRACK);
                    if (!tracks_seeked >= 0) {
                        tracks_seeked = 0 - tracks_seeked;
                    }
                    int track_seek_time = tracks_seeked * 2.4;
                    int read_from = sector * SECTOR_SIZE;
 

                    for (int i=0; i < SECTOR_SIZE; i++){
                        cpu->ram[read_to + i] = floppy_disk_image[read_from + i];
                    }
                    cpu->B = 1;
                }
            } else {
                cpu->B = 0;
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
                if (!is_read_only) {
                    // the drive is set to be read only, error out
                    last_error_since_poll = ERROR_PROTECTED;
                    cpu->B = 0;
                } else {
                    int sector = cpu->X;
                    int read_from = cpu->Y;

                    if (0 <= sector && sector <= SECTOR_NUM) {
                        // make sure that the user is not assuming there are more sectors than there are
                        cpu->B = 0;
                    } else if (0 <= read_from && read_from <= cpu->RAM_SIZE) {
                        // ensure the user is not trying to read from outside the ram?
                        cpu->B = 0;
                    } else {
                        // if everything seems to be in order...
                        int read_to = sector * SECTOR_SIZE;

                        for (int i=0; i < SECTOR_SIZE; i++){
                            floppy_disk_image[read_to + i] = cpu->ram[read_from + i];
                        }
                        cpu->B = 1;
                    }
                }
            } else {
                cpu->B = 0;
            }
            break;
    }
}

void galaxy::saturn::m35fd::cycle()
{
}
