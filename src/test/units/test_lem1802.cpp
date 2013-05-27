#include <lem1802.hpp>

TEST_CASE("hardware/lem1802/activation", "tests the lem1802's activation") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE_FALSE(lem1802.activated());

    cpu.A = 0;
    cpu.B = 0xdead;
    lem1802.interrupt();

    REQUIRE_FALSE(lem1802.activated());

    for (int i = 0; i < cpu.clock_speed; i++) {
        lem1802.cycle();
    }

    REQUIRE_FALSE(lem1802.activated());

    lem1802.cycle();
    REQUIRE(lem1802.activated());

    cpu.A = 0;
    cpu.B = 0;
    lem1802.interrupt();

    REQUIRE_FALSE(lem1802.activated());
}

TEST_CASE("hardware/lem1802/mem_map_screen", "tests the lem1802's MEM_MAP_SCREEN interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE(lem1802.video_map_address() == 0);

    cpu.A = 0;
    cpu.B = 0x8000;
    lem1802.interrupt();

    REQUIRE(lem1802.video_map_address() == 0x8000);
}

TEST_CASE("hardware/lem1802/mem_map_font", "tests the lem1802's MEM_MAP_FONT interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE(lem1802.font_map_address() == 0);

    cpu.A = 1;
    cpu.B = 0x8000;
    lem1802.interrupt();

    REQUIRE(lem1802.font_map_address() == 0x8000);
}

TEST_CASE("hardware/lem1802/mem_map_palette", "tests the lem1802's MEM_MAP_PALETTE interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE(lem1802.palette_map_address() == 0);

    cpu.A = 2;
    cpu.B = 0x8000;
    lem1802.interrupt();

    REQUIRE(lem1802.palette_map_address() == 0x8000);
}

TEST_CASE("hardware/lem1802/set_border_color", "tests the lem1802's SET_BORDER_COLOR interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    galaxy::saturn::pixel b = lem1802.border();

    REQUIRE(b.r == 0);
    REQUIRE(b.g == 0);
    REQUIRE(b.b == 0);

    cpu.A = 3;
    cpu.B = 6;
    lem1802.interrupt();

    b = lem1802.border();

    REQUIRE(b.r == 170);
    REQUIRE(b.g == 85);
    REQUIRE(b.b == 0);
}

TEST_CASE("hardware/lem1802/mem_dump_font", "tests the lem1802's MEM_DUMP_FONT interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    std::array<std::uint16_t, 256> default_font = {
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

    cpu.A = 4;
    cpu.B = 0xdead;
    lem1802.interrupt();

    bool identical = true;

    auto default_it = default_font.begin();
    auto current_it = cpu.ram.begin();
    std::advance(current_it, 0xdead);
    for (; default_it != default_font.end() && current_it != cpu.ram.end(); ++default_it, ++current_it) {
        if (*current_it != *default_it) {
            identical = false;
            break;
        }
    }

    REQUIRE(identical);
}

TEST_CASE("hardware/lem1802/mem_dump_palette", "tests the lem1802's MEM_DUMP_PALETTE interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    std::array<std::uint16_t, 16> default_palette = {
        0x000, 0x00a, 0x0a0, 0x0aa,
        0xa00, 0xa0a, 0xa50, 0xaaa,
        0x555, 0x55f, 0x5f5, 0x5ff,
        0xf55, 0xf5f, 0xff5, 0xfff
    };

    cpu.A = 5;
    cpu.B = 0xdead;
    lem1802.interrupt();

    bool identical = true;

    auto default_it = default_palette.begin();
    auto current_it = cpu.ram.begin();
    std::advance(current_it, 0xdead);
    for (; default_it != default_palette.end() && current_it != cpu.ram.end(); ++default_it, ++current_it) {
        if (*current_it != *default_it) {
            identical = false;
            break;
        }
    }

    REQUIRE(identical);
}

TEST_CASE("hardware/lem1802/test_image", "test the image() output") {
   galaxy::saturn::dcpu cpu;
   galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));
}

