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

#include "libsaturn.hpp"

void galaxy::dcpu::cycle()
{
    //pass
}

// maybe this should just return void
device& attach_device(std::unique_ptr<device> hw)
{
    devices.push_back(hw);
    hw->cpu = this;
    return *hw;
}

void flash(std::array<std::uint16_t, 0x10000> mem)
{
    ram.swap(mem);
}

void reset()
{
    A = B = C = X = Y = Z = I = J = PC = SP = EX = IA = 0;
    ram.fill(0);
}
