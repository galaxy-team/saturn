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

#ifndef M35FD_HPP
#define M35FD_HPP

#include <libsaturn.hpp>
#include <device.hpp>

#include <cstdint>

namespace galaxy {
    namespace saturn {
        /**
         * represents a m35fd hardware device
         */
        class m35fd : public device {
        protected:

            // TODO make these enum class'
            enum fd_states {
                STATE_NO_MEDIA = 0x0000, // There's no floppy in the drive.
                STATE_READY    = 0x0001, // The drive is ready to accept commands.
                STATE_READY_WP = 0x0002, // Same as ready, except the floppy is write protected.
                STATE_BUSY     = 0x0003 // The drive is busy either reading or writing a sector.
            };

            enum error_codes {
                ERROR_NONE        = 0x0000, // There's been no error since the last poll.
                ERROR_BUSY        = 0x0001, // Drive is busy performing an action.
                ERROR_NO_MEDIA    = 0x0002, // Attempted to read or write with no floppy inserted.
                ERROR_PROTECTED   = 0x0003, // Attempted to write to write protected floppy.
                ERROR_EJECT       = 0x0004, // The floppy was removed while reading or writing.
                ERROR_BAD_SECTOR  = 0x0005, // The requested sector is broken, the data on it is lost.
                ERROR_BROKEN      = 0xfffff // There's some major software or hardware problem,
                                            // try turning off and turning the device on again.
            };

            int last_error_since_poll = ERROR_NONE;
            int current_state;
            int interrupt_message;

            // we had to record the track so we can implement the track seek delay
            int current_track = 0;

            const static int SECTOR_SIZE = 512;
            const static int SECTOR_NUM = 1440;

            const static int TRACKS = 80;
            const static int SECTORS_PER_TRACK = 18;

        public:
            /// initialize the device to values specified by the spec
            m35fd() : device(0x4fd524c5, 0x1eb37e91, 0x000b, "Mackapar 3.5\" Floppy Drive (M35FD)") {}


            virtual void interrupt();
            virtual void cycle();
 
            const static int FLOPPY_SIZE = 737280;
            std::array<std::uint16_t, FLOPPY_SIZE> floppy_disk_image;
            bool disk_loaded = false;
            bool is_read_only = false;
        };
    }
}

#endif
