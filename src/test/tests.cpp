#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <libsaturn.hpp>
#include "test_device.hpp"

/// runs the cpu until it encounters an invalid opcode
int execute(galaxy::saturn::dcpu& cpu) {
    int cycles = 0;
    try {
        while (true) {
            cpu.cycle();
            cycles++;
        }
    } catch(galaxy::saturn::invalid_opcode& e) {}
    return cycles;
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

#include "units/test_default.cpp"

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

//#include "units/test_opcodes.cpp"
/**/
TEST_CASE("opcodes/invalid", "a runtime error should be thrown on invalid opcode") {
    galaxy::saturn::dcpu cpu;

    REQUIRE_THROWS_AS(cpu.cycle(), galaxy::saturn::invalid_opcode);
}

TEST_CASE("opcodes/set", "sets b to a") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/add", "sets b to b+a, sets EX to 0x0001 if there's an overflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x435;
    std::vector<std::uint16_t> codez = {0x7c02, 0x0234};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0669);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 3);

    cpu.reset();

    cpu.A = 0xf32d;
    codez = {0x7c02, 0xfed};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x031a);
    REQUIRE(cpu.EX == 0x0001);
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/sub", "sets b to b-a, sets EX to 0xffff if there's an underflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xf32d;
    std::vector<std::uint16_t> codez = {0x7c03, 0xfed};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xe340);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 3);

    cpu.reset();

    cpu.A = 0x435;
    codez = {0x7c03, 0x0500};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xff35);
    REQUIRE(cpu.EX == 0xffff);
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/mul", "sets b to b*a, sets EX to ((b*a)>>16)&0xffff (treats b, a as unsigned)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x234;
    std::vector<std::uint16_t> codez = {0x7c04, 0xfffe};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfb98);
    REQUIRE(cpu.EX == 0x0233);
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/mli", "like MUL, but treat b, a as signed") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x234;
    std::vector<std::uint16_t> codez = {0x7c05, 0xfffe};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfb98);
    REQUIRE(cpu.EX == 0xffff);
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/div", "sets b to b/a, sets EX to ((b<<16)/a)&0xffff. if a==0, sets b and EX to 0 instead. (treats b, a as unsigned)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1231;
    std::vector<std::uint16_t> codez = {0x7c06, 0x000f};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0136);
    REQUIRE(cpu.EX == 0x7777);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0x15;
    codez = {0x7c06, 0xffff};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0015);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0x23;
    codez = {0x7c06, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c06, 0x00df};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);

    REQUIRE(cpu.A == 0xff);
    REQUIRE(cpu.EX == 0xa0b7);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/dvi", "like DIV, but treat b, a as signed. Rounds towards 0") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1231;
    std::vector<std::uint16_t> codez = {0x7c07, 0x000f};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0136);
    REQUIRE(cpu.EX == 0x7777);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0x15;
    codez = {0x7c07, 0xffff};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xffeb);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0x23;
    codez = {0x7c07, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c07, 0x00df};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);

    REQUIRE(cpu.A == 0xffda);
    REQUIRE(cpu.EX == 0xa0b7);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/mod", "sets b to b%a. if a==0, sets b to 0 instead.") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1231;
    std::vector<std::uint16_t> codez = {0x7c08, 0x000f};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0007);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xfff9;
    codez = {0x7c08, 0x0016};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x000d);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0x23;
    codez = {0x7c08, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/mdi", "like MOD, but treat b, a as signed. (MDI -7, 16 == -7)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1231;
    std::vector<std::uint16_t> codez = {0x7c09, 0x000f};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0007);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xfff9;
    codez = {0x7c09, 0x0016};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfff9);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0x23;
    codez = {0x7c09, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/and", "sets b to b&a") {
    galaxy::saturn::dcpu cpu;

    /// 0b0101 0101 0101 0101 & 0b0011 0011 0011 0011 == 0b0001 0001 0001 0001
    cpu.A = 0x5555;
    std::vector<std::uint16_t> codez = {0x7c0a, 0x3333};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x1111);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/bor", "sets b to b|a") {
    galaxy::saturn::dcpu cpu;

    /// 0b0101 0101 0101 0101 | 0b0011 0011 0011 0011 == 0b0111 0111 0111 0111
    cpu.A = 0x5555;
    std::vector<std::uint16_t> codez = {0x7c0b, 0x3333};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x7777);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/xor", "sets b to b^a") {
    galaxy::saturn::dcpu cpu;

    /// 0b0101 0101 0101 0101 ^ 0b0011 0011 0011 0011 == 0b0110 0110 0110 0110
    cpu.A = 0x5555;
    std::vector<std::uint16_t> codez = {0x7c0c, 0x3333};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x6666);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/shr", "sets b to b>>>a, sets EX to ((b<<16)>>a)&0xffff (logical shift)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xdead;
    std::vector<std::uint16_t> codez = {0x7c0d, 0x0004};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0dea);
    REQUIRE(cpu.EX == 0xd000);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/asr", "sets b to b>>a, sets EX to ((b<<16)>>>a)&0xffff (arithmetic shift) (treats b as signed)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xdead;
    std::vector<std::uint16_t> codez = {0x7c0e, 0x0004};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfdea);
    REQUIRE(cpu.EX == 0xd000);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/shl", "sets b to b<<a, sets EX to ((b<<a)>>16)&0xffff") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xdead;
    std::vector<std::uint16_t> codez = {0x7c0f, 0x0004};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xead0);
    REQUIRE(cpu.EX == 0x000d);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/ifb", "performs next instruction only if (b&a)!=0") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0;
    std::vector<std::uint16_t> codez = {0x7c10, 0x0000, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c10, 0x0000, 0x7c01, 0x0000};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c10, 0x0f00, 0x7c01, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cycles == 5);
}

TEST_CASE("opcodes/ifc", "performs next instruction only if (b&a)==0") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0;
    std::vector<std::uint16_t> codez = {0x7c11, 0x0000, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 5);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c11, 0x0000, 0x7c01, 0x0000};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cycles == 5);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c11, 0x0f00, 0x7c01, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/ife", "performs next instruction only if b==a") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1234;
    std::vector<std::uint16_t> codez = {0x7c12, 0x1234, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 5);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c12, 0x0f00, 0x7c01, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/ifn", "performs next instruction only if b!=a") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1234;
    std::vector<std::uint16_t> codez = {0x7c13, 0x1234, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x1234);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xdead;
    codez = {0x7c13, 0x0f00, 0x7c01, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cycles == 5);
}

TEST_CASE("opcodes/ifg", "performs next instruction only if b>a") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0f000;
    std::vector<std::uint16_t> codez = {0x7c14, 0xfffe, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0f000);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xfffe;
    codez = {0x7c14, 0x0f00, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 5);
}

TEST_CASE("opcodes/ifa", "performs next instruction only if b>a (signed)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0f00;
    std::vector<std::uint16_t> codez = {0x7c15, 0xfffe, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 5);

    cpu.reset();

    cpu.A = 0xfffe;
    codez = {0x7c15, 0x0f00, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfffe);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/ifl", "performs next instruction only if b<a") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0f00;
    std::vector<std::uint16_t> codez = {0x7c16, 0xfffe, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 5);

    cpu.reset();

    cpu.A = 0xfffe;
    codez = {0x7c16, 0x0f00, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfffe);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/ifu", "performs next instruction only if b<a (signed)") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0f00;
    std::vector<std::uint16_t> codez = {0x7c17, 0xfffe, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0f00);
    REQUIRE(cycles == 4);

    cpu.reset();

    cpu.A = 0xfffe;
    codez = {0x7c17, 0x0f00, 0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 5);
}

TEST_CASE("opcodes/adx", "sets b to b+a+EX, sets EX to 0x0001 if there is an overflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1234;
    cpu.B = 0x5678;
    cpu.EX = 0xf00;
    std::vector<std::uint16_t> codez = {0x041a};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x77ac);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 3);

    cpu.reset();

    cpu.A = 0xfade;
    cpu.B = 0x5678;
    cpu.EX = 0x0f00;
    codez = {0x041a};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x6056);
    REQUIRE(cpu.EX == 0x0001);
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/sbx", "sets b to b-a+EX, sets EX to 0xFFFF if there is an underflow, 0x0001 if there's an overflow, 0x0 otherwise") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xfade;
    cpu.B = 0x5678;
    cpu.EX = 0xf00;
    std::vector<std::uint16_t> codez = {0x041b};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xb366);
    REQUIRE(cpu.EX == 0x0);
    REQUIRE(cycles == 3);

    cpu.reset();

    cpu.A = 0x0f00;
    cpu.B = 0x5678;
    cpu.EX = 0xf00;
    codez = {0x041b};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xc788);
    REQUIRE(cpu.EX == 0xffff);
    REQUIRE(cycles == 3);

    cpu.reset();

    cpu.A = 0xffff;
    cpu.B = 0x0;
    cpu.EX = 0x1;
    codez = {0x041b};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.EX == 0x0001);
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/sti", "sets b to a, then increases I and J by 1") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xf00;
    cpu.B = 0xdead;
    std::vector<std::uint16_t> codez = {0x041e};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cpu.I == 0x1);
    REQUIRE(cpu.J == 0x1);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/std", "sets b to a, then decreases I and J by 1") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0xf00;
    cpu.B = 0xdead;
    std::vector<std::uint16_t> codez = {0x041f};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cpu.I == 0xffff);
    REQUIRE(cpu.J == 0xffff);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/jsr", "pushes the address of the next instruction to the stack, then sets PC to a") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7c20, 0xdeac};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.PC == 0xdead);
    REQUIRE(cpu.ram[cpu.SP] == 0x2);
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/int", "triggers a software interrupt with message a") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7d00, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cpu.PC == 0x3);
    REQUIRE_FALSE(cpu.interrupt_queue_enabled());
    REQUIRE(cycles == 5);

    cpu.reset();

    cpu.IA = 0x0f00;
    codez = {0x7d00, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cpu.PC == 0x0f01);
    REQUIRE(cpu.interrupt_queue_enabled());
    REQUIRE(cycles == 5);
}

TEST_CASE("opcodes/iag", "sets a to IA") {
    galaxy::saturn::dcpu cpu;

    cpu.IA = 0xdead;
    std::vector<std::uint16_t> codez = {0x0120};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 1);
}

TEST_CASE("opcodes/ias", "sets IA to a") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7d40, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.IA == 0xdead);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/rfi", "disables interrupt queueing, pops A from the stack, then pops PC from the stack") {
    galaxy::saturn::dcpu cpu;

    cpu.ram[--cpu.SP] = 0xdeac;
    cpu.ram[--cpu.SP] = 0xfade;
    std::vector<std::uint16_t> codez = {0x7d60, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xfade);
    REQUIRE(cpu.PC == 0xdead);
    REQUIRE_FALSE(cpu.interrupt_queue_enabled());
    REQUIRE(cycles == 4);
}

TEST_CASE("opcodes/iaq", "if a is nonzero, interrupts will be added to the queue instead of triggered. if a is zero, interrupts will be triggered as normal again") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7d80, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.interrupt_queue_enabled());
    REQUIRE(cycles == 3);

    cpu.reset();

    codez = {0x7d80, 0x0};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE_FALSE(cpu.interrupt_queue_enabled());
    REQUIRE(cycles == 3);
}

TEST_CASE("opcodes/hwn", "sets a to number of connected hardware devices") {
    galaxy::saturn::dcpu cpu;

    for (int i = 0; i < 10; i++) {
        cpu.attach_device(new test_device());
    }

    std::vector<std::uint16_t> codez = {0x0200};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 10);
    REQUIRE(cycles == 2);
}

TEST_CASE("opcodes/hwq", "sets A, B, C, X, Y registers to information about hardware a | A(B<<16) is a 32 bit word identifying the hardware id | C is the hardware version | X+(Y<<16) is a 32 bit word identifying the manufacturer") {
    galaxy::saturn::dcpu cpu;

    test_device& dev = static_cast<test_device&>(cpu.attach_device(new test_device()));
    dev.id = 0x12345678;
    dev.manufacturer = 0xfadedead;
    dev.version = 0x0f00;

    std::vector<std::uint16_t> codez = {0x7e20, 0x0000};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);

    REQUIRE(cpu.B == 0x1234);
    REQUIRE(cpu.A == 0x5678);

    REQUIRE(cpu.C == 0x0f00);

    REQUIRE(cpu.Y == 0xfade);
    REQUIRE(cpu.X == 0xdead);

    REQUIRE(cycles == 5);
}

TEST_CASE("opcodes/hwi", "sends an interrupt to hardware a") {
    galaxy::saturn::dcpu cpu;

    test_device& dev = static_cast<test_device&>(cpu.attach_device(new test_device()));
    std::vector<std::uint16_t> codez = {0x7e40, 0x0000};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);

    REQUIRE(dev.count_interrupts == 0x1);
    REQUIRE(cycles == 5);
}
/**/

TEST_CASE("values/registers", "check registers") {
    galaxy::saturn::dcpu cpu;
    std::vector<std::uint16_t> codez = {0x7C01, 0xDEAD, 0x7C21, 0xDEAD, 0x7C41, 0xDEAD, 0x7C61, 0xDEAD, 0x7C81, 0xDEAD, 0x7CA1, 0xDEAD, 0x7CC1, 0xDEAD, 0x7CE1, 0xDEAD};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cpu.B == 0xdead);
    REQUIRE(cpu.C == 0xdead);
    REQUIRE(cpu.X == 0xdead);
    REQUIRE(cpu.Y == 0xdead);
    REQUIRE(cpu.Z == 0xdead);
    REQUIRE(cpu.I == 0xdead);
    REQUIRE(cpu.J == 0xdead);
    REQUIRE(cycles == 16);
}

TEST_CASE("values/register-pointers", "check pointers in registers") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x0fff;
    cpu.B = 0x1000;
    cpu.C = 0x1001;
    cpu.X = 0x1002;
    cpu.Y = 0x1003;
    cpu.Z = 0x1004;
    cpu.I = 0x1005;
    cpu.J = 0x1006;
    std::vector<std::uint16_t> codez = {0x7E01, 0xDEAD, 0x0001, 0x7E21, 0xDEAD, 0x0001, 0x7E41, 0xDEAD, 0x0001, 0x7E61, 0xDEAD, 0x0001, 0x7E81, 0xDEAD, 0x0001, 0x7EA1, 0xDEAD, 0x0001, 0x7EC1, 0xDEAD, 0x0001, 0x7EE1, 0xDEAD, 0x0001};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.ram[0x1000] == 0xdead);
    REQUIRE(cpu.ram[0x1001] == 0xdead);
    REQUIRE(cpu.ram[0x1002] == 0xdead);
    REQUIRE(cpu.ram[0x1003] == 0xdead);
    REQUIRE(cpu.ram[0x1004] == 0xdead);
    REQUIRE(cpu.ram[0x1005] == 0xdead);
    REQUIRE(cpu.ram[0x1006] == 0xdead);
    REQUIRE(cpu.ram[0x1007] == 0xdead);
    REQUIRE(cycles == 24);
}

TEST_CASE("values/register-nextword", "check [register + next word]") {
    galaxy::saturn::dcpu cpu;

    cpu.A = 0x1000;
    cpu.B = 0x1001;
    cpu.C = 0x1002;
    cpu.X = 0x1003;
    cpu.Y = 0x1004;
    cpu.Z = 0x1005;
    cpu.I = 0x1006;
    cpu.J = 0x1007;
    std::vector<std::uint16_t> codez = {0x7D01, 0xDEAD, 0x7D21, 0xDEAD, 0x7D41, 0xDEAD, 0x7D61, 0xDEAD, 0x7D81, 0xDEAD, 0x7DA1, 0xDEAD, 0x7DC1, 0xDEAD, 0x7DE1, 0xDEAD};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.ram[0x1000] == 0xdead);
    REQUIRE(cpu.ram[0x1001] == 0xdead);
    REQUIRE(cpu.ram[0x1002] == 0xdead);
    REQUIRE(cpu.ram[0x1003] == 0xdead);
    REQUIRE(cpu.ram[0x1004] == 0xdead);
    REQUIRE(cpu.ram[0x1005] == 0xdead);
    REQUIRE(cpu.ram[0x1006] == 0xdead);
    REQUIRE(cpu.ram[0x1007] == 0xdead);
    REQUIRE(cycles == 16);
}

TEST_CASE("values/stack", "check stack") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7f01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.SP == 0xffff);
    REQUIRE(cpu.ram[cpu.SP] == 0xdead);
    REQUIRE(cycles == 2);

    cpu.reset();

    cpu.ram[--cpu.SP] = 0xdead;
    codez = {0x6001};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.SP == 0x0);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 1);

    cpu.reset();

    cpu.ram[--cpu.SP] = 0xdead;
    codez = {0x6401};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.SP == 0xffff);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 1);

    cpu.reset();

    cpu.ram[--cpu.SP] = 0xdead;
    cpu.ram[--cpu.SP] = 0xfade;
    cpu.ram[--cpu.SP] = 0xcafe;
    codez = {0x6801, 0x0002};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.SP == 0xfffd);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 2);
}

TEST_CASE("values/special", "check special memory locations") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7f61, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.SP == 0xdead);
    REQUIRE(cycles == 2);

    cpu.reset();

    codez = {0x7f81, 0xdeac};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.PC == 0xdead);
    REQUIRE(cycles == 2);

    cpu.reset();

    codez = {0x7fa1, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    cycles = execute(cpu);
    REQUIRE(cpu.EX == 0xdead);
    REQUIRE(cycles == 2);
}


TEST_CASE("values/next-word-pointer", "check next word pointer addressing") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7fc1, 0xdead, 0x8000};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.ram[0x8000] == 0xdead);
    REQUIRE(cycles == 3);
}

TEST_CASE("values/next-word-literal", "check next word literals") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7c01, 0xdead};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0xdead);
    REQUIRE(cycles == 2);
}

TEST_CASE("values/short-literals", "check short form literals") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0xd801};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x15);
    REQUIRE(cycles == 1);
}

TEST_CASE("conditionals/skipping", "check conditional skipping") {
    galaxy::saturn::dcpu cpu;

    std::vector<std::uint16_t> codez = {0x7c12, 0xdead, 0x7c32, 0xdead, 0x7c01, 0x0f00};
    cpu.flash(codez.begin(), codez.end());
    int cycles = execute(cpu);
    REQUIRE(cpu.A == 0x0);
    REQUIRE(cycles == 5);
}

TEST_CASE("interrupts/queueing", "check interrupt queueing") {
    galaxy::saturn::dcpu cpu;

    cpu.IA = 0x1;
    cpu.interrupt(0x21);
    cpu.interrupt(0x3f);
    cpu.interrupt(0x13);
    cpu.ram[0x1] = 0x0022;
    cpu.ram[0x2] = 0x7d60;
    cpu.ram[0x3] = 0x0f00;
    execute(cpu);
    REQUIRE(cpu.B == 0x73);
}

TEST_CASE("interrupts/overflow", "an exception should be thrown on interrupt queue overflow") {
    galaxy::saturn::dcpu cpu;

    cpu.IA = 0x1;

    // the first interrupt triggered is not added to the queue, so trigger 257 instead of 256
    for (int i = 0; i < 257; i++) {
        cpu.interrupt(0xfab);
    }

    REQUIRE_THROWS_AS(cpu.interrupt(0xf00), galaxy::saturn::queue_overflow);
}

TEST_CASE("hardware/cycles", "a device's cycle() method must be called once per cycle") {
    galaxy::saturn::dcpu cpu;

    test_device& dev = static_cast<test_device&>(cpu.attach_device(new test_device()));
    std::vector<std::uint16_t> codez = {0x7f81, 0x0000};
    cpu.flash(codez.begin(), codez.end());

    for (int i = 0; i < 0xfade; i++) {
        cpu.cycle();
    }

    REQUIRE(dev.count_cycles == 0xfade);
}

TEST_CASE("clock/tick_count", "test interrupt #1") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::clock& clock = static_cast<galaxy::saturn::clock&>(cpu.attach_device(new galaxy::saturn::clock()));

    REQUIRE(cpu.clock_speed == 100000);

    cpu.A = 0;
    cpu.B = 120;
    clock.interrupt();

    // call clock.cycle() for 10 seconds
    for (int i = 0; i < cpu.clock_speed * 10; i++) {
        clock.cycle();
    }

    cpu.A = 1;
    clock.interrupt();

    REQUIRE(cpu.C == 5);

    cpu.A = 0;
    cpu.B = 60;
    clock.interrupt();

    cpu.A = 1;
    clock.interrupt();

    REQUIRE(cpu.C == 0);
}

TEST_CASE("clock/tick_interrupt", "test interupt #2") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::clock& clock = static_cast<galaxy::saturn::clock&>(cpu.attach_device(new galaxy::saturn::clock()));

    cpu.A = 0;
    cpu.B = 60;
    clock.interrupt();

    cpu.A = 2;
    cpu.B = 0x3;
    clock.interrupt();

    cpu.IA = 0x2;
    cpu.I = 0x0;
    std::vector<std::uint16_t> codez = {0x7f81, 0x0000, 0x00c2, 0x7d60, 0xdead};
    cpu.flash(codez.begin(), codez.end());

    for (int i = 0; i < cpu.clock_speed * 3 + 1; i++) {
        cpu.cycle();
    }

    REQUIRE(cpu.I == 9);
}

#include "units/test_m35fd.cpp"

TEST_CASE("keyboard/buffer", "test the keyboard's internal buffer") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::keyboard& keyboard = static_cast<galaxy::saturn::keyboard&>(cpu.attach_device(new galaxy::saturn::keyboard()));

    cpu.A = 1;
    cpu.C = 0xdead;
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);

    // non-ascii
    keyboard.press(0x11);
    cpu.A = 1;
    keyboard.interrupt();
    REQUIRE(cpu.C == 0x11);
    keyboard.release(0x11);
    keyboard.interrupt();
    REQUIRE(cpu.C == 0x11);
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);

    // ascii
    keyboard.press(0x21);
    cpu.A = 1;
    keyboard.interrupt();
    REQUIRE(cpu.C == 0x21);
    keyboard.release(0x21);
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);

    cpu.C = 0xdead;

    // test interrupt 0
    keyboard.press(0x11);
    keyboard.release(0x11);
    keyboard.press(0x31);
    keyboard.release(0x31);
    keyboard.press(0x13);
    keyboard.release(0x13);
    cpu.A = 0;
    keyboard.interrupt();
    cpu.A = 1;
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);
}

TEST_CASE("keyboard/key-pressed", "test the keyboard's ability to check if a key is pressed") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::keyboard& keyboard = static_cast<galaxy::saturn::keyboard&>(cpu.attach_device(new galaxy::saturn::keyboard()));

    cpu.C = 0xdead;

    cpu.A = 2;
    cpu.B = 0x11;
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);

    keyboard.press(0x11);
    cpu.A = 2;
    cpu.B = 0x11;
    keyboard.interrupt();
    REQUIRE(cpu.C == 1);
    keyboard.release(0x11);
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);

    keyboard.press(0x31);
    cpu.A = 2;
    cpu.B = 0x31;
    keyboard.interrupt();
    REQUIRE(cpu.C == 1);
    keyboard.release(0x31);
    keyboard.interrupt();
    REQUIRE(cpu.C == 0);
}

TEST_CASE("keyboard/interrupts", "test the keyboard's event interrupts") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::keyboard& keyboard = static_cast<galaxy::saturn::keyboard&>(cpu.attach_device(new galaxy::saturn::keyboard()));

    cpu.IA = 0x2;
    cpu.I = 0x0;
    std::vector<std::uint16_t> codez = {0x7f81, 0x0000, 0x00c2, 0x7d60, 0xdead};
    cpu.flash(codez.begin(), codez.end());

    cpu.A = 3;
    cpu.B = 0x1;
    keyboard.interrupt();

    keyboard.press(0x11);
    keyboard.press(0x31);
    keyboard.release(0x11);
    keyboard.press(0x90);
    keyboard.release(0x90);
    keyboard.release(0x31);

    for (int i = 0; i < 50; i++) {
        cpu.cycle();
    }

    REQUIRE(cpu.I == 0x5);
}

TEST_CASE("lem1802/activation", "tests the lem1802's activation") {
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

TEST_CASE("lem1802/mem_map_screen", "tests the lem1802's MEM_MAP_SCREEN interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE(lem1802.video_map_address() == 0);

    cpu.A = 0;
    cpu.B = 0x8000;
    lem1802.interrupt();

    REQUIRE(lem1802.video_map_address() == 0x8000);
}

TEST_CASE("lem1802/mem_map_font", "tests the lem1802's MEM_MAP_FONT interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE(lem1802.font_map_address() == 0);

    cpu.A = 1;
    cpu.B = 0x8000;
    lem1802.interrupt();

    REQUIRE(lem1802.font_map_address() == 0x8000);
}

TEST_CASE("lem1802/mem_map_palette", "tests the lem1802's MEM_MAP_PALETTE interrupt") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::lem1802& lem1802 = static_cast<galaxy::saturn::lem1802&>(cpu.attach_device(new galaxy::saturn::lem1802()));

    REQUIRE(lem1802.palette_map_address() == 0);

    cpu.A = 2;
    cpu.B = 0x8000;
    lem1802.interrupt();

    REQUIRE(lem1802.palette_map_address() == 0x8000);
}

TEST_CASE("lem1802/set_border_color", "tests the lem1802's SET_BORDER_COLOR interrupt") {
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

TEST_CASE("lem1802/mem_dump_font", "tests the lem1802's MEM_DUMP_FONT interrupt") {
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

TEST_CASE("lem1802/mem_dump_palette", "tests the lem1802's MEM_DUMP_PALETTE interrupt") {
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

TEST_CASE("sped3/initialize", "the sped3 should initialize in STATE_NO_DATA and with ERROR_NONE") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::sped3& sped = static_cast<galaxy::saturn::sped3&>(cpu.attach_device(new galaxy::saturn::sped3()));

    cpu.A = 0;
    sped.interrupt();

    REQUIRE(cpu.B == galaxy::saturn::sped3::STATE_NO_DATA);
    REQUIRE(cpu.C == galaxy::saturn::sped3::ERROR_NONE);
}

TEST_CASE("sped3/state_running", "the sped3 should set its state to STATE_RUNNING when it is projecting lines") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::sped3& sped = static_cast<galaxy::saturn::sped3&>(cpu.attach_device(new galaxy::saturn::sped3()));

    cpu.A = 1;
    cpu.X = 0xdead;
    cpu.Y = 5;
    sped.interrupt();

    cpu.A = 0;
    sped.interrupt();

    REQUIRE(cpu.B == galaxy::saturn::sped3::STATE_RUNNING);
    REQUIRE(cpu.C == galaxy::saturn::sped3::ERROR_NONE);
}

TEST_CASE("sped3/model", "tests the sped3's interrupt #1 and vertices() function") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::sped3& sped = static_cast<galaxy::saturn::sped3&>(cpu.attach_device(new galaxy::saturn::sped3()));

    cpu.A = 0x1;
    cpu.X = 0x0;
    cpu.Y = 7;
    std::vector<std::uint16_t> codez = {0x505, 0x100, 0x8282, 0x658, 0xfafa, 0x300, 0x5fa, 0x500, 0x8282, 0x258, 0xfa05, 0x700, 0x505, 0x100};
    cpu.flash(codez.begin(), codez.end());

    sped.interrupt();
    std::vector<galaxy::saturn::vertex> pyramid = sped.vertices();

    REQUIRE(pyramid[0].x == 5);
    REQUIRE(pyramid[0].y == 5);
    REQUIRE(pyramid[0].z == 0);
    REQUIRE_FALSE(pyramid[0].intense);
    REQUIRE(pyramid[0].color == galaxy::saturn::vertex::COLOR_RED);

    REQUIRE(pyramid[1].x == 130);
    REQUIRE(pyramid[1].y == 130);
    REQUIRE(pyramid[1].z == 88);
    REQUIRE(pyramid[1].intense);
    REQUIRE(pyramid[1].color == galaxy::saturn::vertex::COLOR_GREEN);

    REQUIRE(pyramid[2].x == 250);
    REQUIRE(pyramid[2].y == 250);
    REQUIRE(pyramid[2].z == 0);
    REQUIRE_FALSE(pyramid[2].intense);
    REQUIRE(pyramid[2].color == galaxy::saturn::vertex::COLOR_BLUE);

    REQUIRE(pyramid[3].x == 250);
    REQUIRE(pyramid[3].y == 5);
    REQUIRE(pyramid[3].z == 0);
    REQUIRE(pyramid[3].intense);
    REQUIRE(pyramid[3].color == galaxy::saturn::vertex::COLOR_RED);

    REQUIRE(pyramid[4].x == 130);
    REQUIRE(pyramid[4].y == 130);
    REQUIRE(pyramid[4].z == 88);
    REQUIRE_FALSE(pyramid[4].intense);
    REQUIRE(pyramid[4].color == galaxy::saturn::vertex::COLOR_GREEN);

    REQUIRE(pyramid[5].x == 5);
    REQUIRE(pyramid[5].y == 250);
    REQUIRE(pyramid[5].z == 0);
    REQUIRE(pyramid[5].intense);
    REQUIRE(pyramid[5].color == galaxy::saturn::vertex::COLOR_BLUE);

    REQUIRE(pyramid[6].x == 5);
    REQUIRE(pyramid[6].y == 5);
    REQUIRE(pyramid[6].z == 0);
    REQUIRE_FALSE(pyramid[6].intense);
    REQUIRE(pyramid[6].color == galaxy::saturn::vertex::COLOR_RED);
}

TEST_CASE("sped3/state_turning", "the sped3 should set its state to STATE_TURNING when it is rotating") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::sped3& sped = static_cast<galaxy::saturn::sped3&>(cpu.attach_device(new galaxy::saturn::sped3()));

    cpu.A = 2;
    cpu.X = 200;
    sped.interrupt();

    cpu.A = 0;
    sped.interrupt();

    REQUIRE(cpu.B == galaxy::saturn::sped3::STATE_TURNING);
    REQUIRE(cpu.C == galaxy::saturn::sped3::ERROR_NONE);

    for (int i = 0; i < cpu.clock_speed * 4; i++) {
        sped.cycle();
    }

    cpu.A = 0;
    sped.interrupt();

    REQUIRE(cpu.B == galaxy::saturn::sped3::STATE_NO_DATA);
    REQUIRE(cpu.C == galaxy::saturn::sped3::ERROR_NONE);

    cpu.A = 1;
    cpu.Y = 10;
    sped.interrupt();

    cpu.A = 2;
    cpu.X = 0;
    sped.interrupt();

    cpu.A = 0;
    sped.interrupt();

    REQUIRE(cpu.B == galaxy::saturn::sped3::STATE_TURNING);
    REQUIRE(cpu.C == galaxy::saturn::sped3::ERROR_NONE);

    for (int i = 0; i < cpu.clock_speed * 4; i++) {
        sped.cycle();
    }

    cpu.A = 0;
    sped.interrupt();

    REQUIRE(cpu.B == galaxy::saturn::sped3::STATE_RUNNING);
    REQUIRE(cpu.C == galaxy::saturn::sped3::ERROR_NONE);
}

TEST_CASE("sped3/rotation", "tests the sped3's rotation") {
    galaxy::saturn::dcpu cpu;
    galaxy::saturn::sped3& sped = static_cast<galaxy::saturn::sped3&>(cpu.attach_device(new galaxy::saturn::sped3()));

    REQUIRE(sped.rotation() == 0);

    cpu.A = 2;
    cpu.X = 100;
    sped.interrupt();

    REQUIRE(sped.rotation() == 0);

    for (int i = 0; i < cpu.clock_speed; i++) {
        sped.cycle();
    }

    REQUIRE(sped.rotation() == sped.ROTATION_SPEED);

    cpu.A = 2;
    cpu.X = 320;
    sped.interrupt();

    for (int i = 0; i < cpu.clock_speed; i++) {
        sped.cycle();
    }

    REQUIRE(sped.rotation() == 0);

    cpu.A = 2;
    cpu.X = 800;
    sped.interrupt();

    for (int i = 0; i < cpu.clock_speed * 9; i++) {
        sped.cycle();
    }

    REQUIRE(sped.rotation() == 80);
}
