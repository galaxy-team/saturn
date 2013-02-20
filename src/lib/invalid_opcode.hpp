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

#ifndef INVALID_OPCODE_HPP
#define INVALID_OPCODE_HPP

namespace galaxy {
    namespace saturn {
        /**
         * throw when the DCPU attempts to execute an invalid opcode
         */
        class invalid_opcode : std::runtime_error {
            public:
                invalid_opcode(const std::string& msg) : runtime_error(msg) {}
        };
    }
}

#endif
