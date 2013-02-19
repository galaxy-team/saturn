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
    } catch(std::exception& e) {}
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

TEST_CASE("defaults/registers", "All registers should initially be set to 0") {
    galaxy::saturn::dcpu cpu;
    check_registers(cpu);
}

TEST_CASE("defaults/memory", "All memory should initially be set to 0") {
    galaxy::saturn::dcpu cpu;
    check_memory(cpu);
}

TEST_CASE("defaults/interrupt_queue", "The interrupt queue should initially be disabled") {
    galaxy::saturn::dcpu cpu;
    REQUIRE_FALSE(cpu.interrupt_queue_enabled());
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

    // TODO: overflow test?
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
