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

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include "libjupiter.hpp"

namespace galaxy {
    namespace saturn {
        /**
         * a device represents a DCPU hardware device
         */
        class device {

          public:

            std::uint32_t id;
            std::uint32_t manufacturer;
            std::uint16_t version;
            std::string name;

            dcpu cpu;

            /**
             * called at approximately 60 Hz
             */
            virtual void tick() = 0;

            /**
             * called when a hardware interrupt is sent to the device
             */
            virtual void handle_interrupt() = 0;
        }
    }
}

#endif
