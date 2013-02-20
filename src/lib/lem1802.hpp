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

#ifndef LEM1802_HPP
#define LEM1802_HPP

#include <device.hpp>

namespace galaxy {
    namespace saturn {
        /**
         * represents a lem1802 hardware device
         */
        class lem1802 {
          public:
            /// initialize the device to values specified by the spec
            lem1802()   :   id(0x7349f615), manufacturer(0x1c6c8b36),
                            version(0x1802), name("LEM1802 - Low Energy Monitor") {}
        };
    }
}

#endif