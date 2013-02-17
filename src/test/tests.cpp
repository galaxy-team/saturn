#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <libsaturn.hpp>

/// runs the cpu until it encounters an invalid opcode
void execute(galaxy::saturn::dcpu& cpu) {
    try {
        while (true) {
            cpu.cycle();
        }
    } catch(std::exception& e) {}
}

void check_registers(galaxy::saturn::dcpu& cpu) {
    REQUIRE(cpu.A == 0);
    REQUIRE(cpu.B == 0);
    REQUIRE(cpu.C == 0);
    REQUIRE(cpu.X == 0);
    REQUIRE(cpu.Y == 0);
    REQUIRE(cpu.Z == 0);
    REQUIRE(cpu.I == 0);
    REQUIRE(cpu.J == 0);
    REQUIRE(cpu.PC == 0);
    REQUIRE(cpu.SP == 0);
    REQUIRE(cpu.EX == 0);
    REQUIRE(cpu.IA == 0);
}

void check_memory(galaxy::saturn::dcpu& cpu) {
    bool all_zero = true;

    for (int i = 0; i < cpu.ram.size(); i++) {
        if (cpu.ram[i] != 0) {
            all_zero = false;
        }
    }

    REQUIRE(all_zero);
}

TEST_CASE("defaults/registers", "All registers should initially be set to 0") {
    galaxy::saturn::dcpu cpu;
    check_registers(cpu);
}

TEST_CASE("defaults/memory", "All memory should initially be set to 0") {
    galaxy::saturn::dcpu cpu;
    check_memory(cpu);
}

TEST_CASE("reset/registers", "All registers should be 0 after a reset") {
    galaxy::saturn::dcpu cpu;
    cpu.A = cpu.B = cpu.C = cpu.X = cpu.Y = cpu.Z = cpu.I = cpu.J = cpu.PC = cpu.SP = cpu.EX = cpu.IA = 0x234;

    cpu.reset();
    check_registers(cpu);
}

TEST_CASE("reset/memory", "All memory should be 0 after a reset") {
    galaxy::saturn::dcpu cpu;
    cpu.ram.fill(0xf00);

    cpu.reset();

    bool all_zero = true;

    for (int i = 0; i < cpu.ram.size(); i++) {
        if (cpu.ram[i] != 0) {
            all_zero = false;
        }
    }

    REQUIRE(all_zero);
}

TEST_CASE("opcodes/set", "sets b to a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/add", "sets b to b+a, sets EX to 0x0001 if there's an overflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x435;
    std::vector<std::uint16_t> codez = {0x7c02, 0x0234};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x0669);
    REQUIRE(cpu.EX == 0x0);

    cpu.reset();

    cpu.A = 0xf32d;
    codez = {0x7c02, 0xfed};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x031a);
    REQUIRE(cpu.EX == 0x0001);
}

TEST_CASE("opcodes/sub", "sets b to b-a, sets EX to 0xffff if there's an underflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xf32d;
    std::vector<std::uint16_t> codez = {0x7c03, 0xfed};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xe340);
    REQUIRE(cpu.EX == 0x0);

    cpu.reset();

    cpu.A = 0x435;
    codez = {0x7c03, 0x0500};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xff35);
    REQUIRE(cpu.EX == 0xffff);
}

TEST_CASE("opcodes/mul", "sets b to b*a, sets EX to ((b*a)>>16)&0xffff (treats b, a as unsigned)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x234;
    std::vector<std::uint16_t> codez = {0x7c04, 0xfffe};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xfb98);
    REQUIRE(cpu.EX == 0x0233);
}

TEST_CASE("opcodes/mli", "like MUL, but treat b, a as signed") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x234;
    std::vector<std::uint16_t> codez = {0x7c05, 0xfffe};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xfb98);
    REQUIRE(cpu.EX == 0xffff);
}

TEST_CASE("opcodes/div", "sets b to b/a, sets EX to ((b<<16)/a)&0xffff. if a==0, sets b and EX to 0 instead. (treats b, a as unsigned)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1231;
    std::vector<std::uint16_t> codez = {0x7c06, 0x000f};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x0136);
    REQUIRE(cpu.EX == 0x7777);

    cpu.reset();

    cpu.A = 0x15;
    codez = {0x7c06, 0xffff};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0015);

    cpu.reset();

    cpu.A = 0x23;
    codez = {0x7c06, 0x0};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0);
}

TEST_CASE("opcodes/dvi", "like DIV, but treat b, a as signed. Rounds towards 0") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1231;
    std::vector<std::uint16_t> codez = {0x7c07, 0x000f};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x0136);
    REQUIRE(cpu.EX == 0x7777);

    cpu.reset();

    cpu.A = 0x15;
    codez = {0x7c07, 0xffff};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xffeb);
    REQUIRE(cpu.EX == 0x0);

    cpu.reset();

    cpu.A = 0x23;
    codez = {0x7c07, 0x0};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0);
}

TEST_CASE("opcodes/mod", "sets b to b%a. if a==0, sets b to 0 instead.") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/mdi", "like MOD, but treat b, a as signed. (MDI -7, 16 == -7)") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/and", "sets b to b&a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/bor", "sets b to b|a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/xor", "sets b to b^a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/shr", "sets b to b>>>a, sets EX to ((b<<16)>>a)&0xffff (logical shift)") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/asr", "sets b to b>>a, sets EX to ((b<<16)>>>a)&0xffff (arithmetic shift) (treats b as signed)") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/shl", "sets b to b<<a, sets EX to ((b<<a)>>16)&0xffff") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifb", "performs next instruction only if (b&a)!=0") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifc", "performs next instruction only if (b&a)==0") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ife", "performs next instruction only if b==a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifn", "performs next instruction only if b!=a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifg", "performs next instruction only if b>a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifa", "performs next instruction only if b>a (signed)") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifl", "performs next instruction only if b<a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/ifu", "performs next instruction only if b<a (signed)") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/adx", "sets b to b+a+EX, sets EX to 0x0001 if there is an overflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/sbx", "sets b to b-a+EX, sets EX to 0xFFFF if there is an underflow, 0x0001 if there's an overflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/sti", "sets b to a, then increases I and J by 1") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}

TEST_CASE("opcodes/std", "sets b to a, then decreases I and J by 1") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    execute(cpu);
    REQUIRE(cpu.A == 0xdead);
}
