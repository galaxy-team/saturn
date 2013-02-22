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
#include <lem1802.hpp>

const std::array<std::uint16_t, 256> galaxy::saturn::lem1802::default_font = {
    0xb79e, 0x388e, 0x722c, 0x75f4, 0x19bb, 0x7f8f, 0x85f9, 0xb158, 0x242e, 0x2400, 0x082a, 0x0800, 0x0008, 0x0000, 0x0808, 0x0808,
    0x00ff, 0x0000, 0x00f8, 0x0808, 0x08f8, 0x0000, 0x080f, 0x0000, 0x000f, 0x0808, 0x00ff, 0x0808, 0x08f8, 0x0808, 0x08ff, 0x0000,
    0x080f, 0x0808, 0x08ff, 0x0808, 0x6633, 0x99cc, 0x9933, 0x66cc, 0xfef8, 0xe080, 0x7f1f, 0x0701, 0x0107, 0x1f7f, 0x80e0, 0xf8fe,
    0x5500, 0xaa00, 0x55aa, 0x55aa, 0xffaa, 0xff55, 0x0f0f, 0x0f0f, 0xf0f0, 0xf0f0, 0x0000, 0xffff, 0xffff, 0x0000, 0xffff, 0xffff,
    0x0000, 0x0000, 0x005f, 0x0000, 0x0300, 0x0300, 0x3e14, 0x3e00, 0x266b, 0x3200, 0x611c, 0x4300, 0x3629, 0x7650, 0x0002, 0x0100,
    0x1c22, 0x4100, 0x4122, 0x1c00, 0x1408, 0x1400, 0x081c, 0x0800, 0x4020, 0x0000, 0x0808, 0x0800, 0x0040, 0x0000, 0x601c, 0x0300,
    0x3e49, 0x3e00, 0x427f, 0x4000, 0x6259, 0x4600, 0x2249, 0x3600, 0x0f08, 0x7f00, 0x2745, 0x3900, 0x3e49, 0x3200, 0x6119, 0x0700,
    0x3649, 0x3600, 0x2649, 0x3e00, 0x0024, 0x0000, 0x4024, 0x0000, 0x0814, 0x2200, 0x1414, 0x1400, 0x2214, 0x0800, 0x0259, 0x0600,
    0x3e59, 0x5e00, 0x7e09, 0x7e00, 0x7f49, 0x3600, 0x3e41, 0x2200, 0x7f41, 0x3e00, 0x7f49, 0x4100, 0x7f09, 0x0100, 0x3e41, 0x7a00,
    0x7f08, 0x7f00, 0x417f, 0x4100, 0x2040, 0x3f00, 0x7f08, 0x7700, 0x7f40, 0x4000, 0x7f06, 0x7f00, 0x7f01, 0x7e00, 0x3e41, 0x3e00,
    0x7f09, 0x0600, 0x3e61, 0x7e00, 0x7f09, 0x7600, 0x2649, 0x3200, 0x017f, 0x0100, 0x3f40, 0x7f00, 0x1f60, 0x1f00, 0x7f30, 0x7f00,
    0x7708, 0x7700, 0x0778, 0x0700, 0x7149, 0x4700, 0x007f, 0x4100, 0x031c, 0x6000, 0x417f, 0x0000, 0x0201, 0x0200, 0x8080, 0x8000,
    0x0001, 0x0200, 0x2454, 0x7800, 0x7f44, 0x3800, 0x3844, 0x2800, 0x3844, 0x7f00, 0x3854, 0x5800, 0x087e, 0x0900, 0x4854, 0x3c00,
    0x7f04, 0x7800, 0x047d, 0x0000, 0x2040, 0x3d00, 0x7f10, 0x6c00, 0x017f, 0x0000, 0x7c18, 0x7c00, 0x7c04, 0x7800, 0x3844, 0x3800,
    0x7c14, 0x0800, 0x0814, 0x7c00, 0x7c04, 0x0800, 0x4854, 0x2400, 0x043e, 0x4400, 0x3c40, 0x7c00, 0x1c60, 0x1c00, 0x7c30, 0x7c00,
    0x6c10, 0x6c00, 0x4c50, 0x3c00, 0x6454, 0x4c00, 0x0836, 0x4100, 0x0077, 0x0000, 0x4136, 0x0800, 0x0201, 0x0201, 0x0205, 0x0200
};

const std::array<std::uint16_t, 16> galaxy::saturn::lem1802::default_palette = {
    0x000, 0x00a, 0x0a0, 0x0aa,
    0xa00, 0xa0a, 0xa50, 0xaaa,
    0x555, 0x55f, 0x5f5, 0x5ff,
    0xf55, 0xf5f, 0xff5, 0xfff
};

void galaxy::saturn::lem1802::interrupt()
{
    switch(cpu->A) {
        /**
         * MEM_MAP_SCREEN
         * Reads the B register, and maps the video ram to DCPU-16 ram starting
         * at address B. If B is 0, the screen is disconnected. When the screen
         * goes from 0 to any other value, the the LEM1802 takes about one
         * second to start up. Other interrupts sent during this time are still
         * processed.
         */
        case 0:
            vram_pointer = cpu->B;
            monitor_updated = true;
            break;

        /**
         * MEM_MAP_FONT
         * Reads the B register, and maps the font ram to DCPU-16 ram starting
         * at address B. If B is 0, the default font is used instead.
         */
        case 1:
            fram_pointer = cpu->B;
            monitor_updated = true;
            break;

        /**
         * MEM_MAP_PALETTE
         * Reads the B register, and maps the palette ram to DCPU-16 ram starting
         * at address B. If B is 0, the default palette is used instead.
         */
        case 2:
            pram_pointer = cpu->B;
            monitor_updated = true;
            break;

        /**
         * SET_BORDER_COLOR
         * Reads the B register, and sets the border color to palette index B&0xF
         */
        case 3:
            border_color = cpu->B & 0xf;
            monitor_updated = true;
            break;

        /**
         * MEM_DUMP_FONT
         * Reads the B register, and writes the default font data to DCPU-16 ram
         * starting at address B.
         * Halts the DCPU-16 for 256 cycles
         */
        case 4:
            {
                auto font_it = default_font.begin();
                auto ram_it = cpu->ram.begin();
                std::advance(ram_it, cpu->B);
                for (; font_it != default_font.end() && ram_it != cpu->ram.end(); ++font_it, ++ram_it)
                {
                    *ram_it = *font_it;
                }
            }
            break;

        /**
         * MEM_DUMP_PALETTE
         * Reads the B register, and writes the default palette data to DCPU-16
         * ram starting at address B.
         * Halts the DCPU-16 for 16 cycles
         */
        case 5:
            {
                auto palette_it = default_palette.begin();
                auto ram_it = cpu->ram.begin();
                std::advance(ram_it, cpu->B);
                for (; palette_it != default_palette.end() && ram_it != cpu->ram.end(); ++palette_it, ++ram_it)
                {
                    *ram_it = *palette_it;
                }
            }
            break;
    }
}

void galaxy::saturn::lem1802::cycle()
{
    if (cycles >= cpu->clock_speed) {
        cycles = 0;
        blink_on = !blink_on;
        monitor_updated = true;
    }
}

bool galaxy::saturn::lem1802::updated()
{
    bool updated_old = monitor_updated;
    monitor_updated = false;
    return updated_old;
}
