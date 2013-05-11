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
        class M35FD : public device {
        protected:

            // TODO make these an enum class'
            enum fd_states {
                STATE_NO_MEDIA, // There's no floppy in the drive.
                STATE_READY,    // The drive is ready to accept commands.
                STATE_READY_WP, // Same as ready, except the floppy is write protected.
                STATE_BUSY      // The drive is busy either reading or writing a sector.
            };

            enum error_codes {
                ERROR_NONE,       // There's been no error since the last poll.
                ERROR_BUSY,       // Drive is busy performing an action.
                ERROR_NO_MEDIA,   // Attempted to read or write with no floppy inserted.
                ERROR_PROTECTED,  // Attempted to write to write protected floppy.
                ERROR_EJECT,      // The floppy was removed while reading or writing.
                ERROR_BAD_SECTOR, // The requested sector is broken, the data on it is lost.
                ERROR_BROKEN      // There's some major software or hardware problem,
                                  // try turning off and turning the device on again.
            };

	   FILE *file;

        public:
            /// initialize the device to values specified by the spec
/*            m35fd() : device(0x7349f615, 0x1c6c8b36, 0x1802, "LEM1802 - Low Energy Monitor"),
                        vram_pointer(0), fram_pointer(0), pram_pointer(0), border_color(0),
                        blink_on(false), state(DISCONNECTED), cycles(0) {}
*/
            virtual void interrupt();
            virtual void cycle();
        };
    }
}

#endif
