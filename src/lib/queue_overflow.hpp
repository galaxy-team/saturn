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

#ifndef QUEUE_OVERFLOW_HPP
#define QUEUE_OVERFLOW_HPP

#include <stdexcept>

namespace galaxy {
    namespace saturn {
        /**
         * throw when the DCPU's interrupt queue overflows
         * the DCPU will normally catch fire at this point
         */
        class queue_overflow : public std::runtime_error {
        public:
            queue_overflow(const std::string& what_str) : runtime_error(what_str) {}
        };
    }
}

#endif
