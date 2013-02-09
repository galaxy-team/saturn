#include <libjupiter.hpp>
#include <cstdint>
#include <iostream>

#define TEST_CASE(name, code, result) \
std::vector<std::uint16_t> name(code); \
dcpu name_cpu(); \
name_cpu.flash(name.begin(), name.end()); \
try { \
    while(true) { \
        name_cpu.cycle(); \
    } \
} \
catch(std::exception e) {} \
if(name_cpu.A != result) { \
    std::cerr << "error: Test case \"" << #name << "\" failed! It returned 0x" << std::hex << name_cpu.A << std::endl; \
    return -1; \
} else { \
    std::cout << "Test case \"" << #name << "\" passed." << std::endl; \
}


int main() {

    TEST_CASE(set, {0x7c01, 0xdead}, 0xdead);
    TEST_CASE(add, {0xd001, 0x7c02, 0x0043}, 0x56);
    TEST_CASE(sub, {0x7c03, 0xfffe}, 0x2);
    TEST_CASE(mul, {0x7c01, 0x342f, 0x7c21, 0x0023, 0x0404}, 0x226d);
    TEST_CASE(mli, {0x7c01, 0x00fe, 0xc005}, 0xee2);
    TEST_CASE(div, {0x7c01, 0xffdb, 0x9806}, 0x332b);
    TEST_CASE(dvi, {0x7c01, 0xffdb, 0x9807}, 0xfff9);
    TEST_CASE(mod, {0x7c01, 0xffdb, 0x9808}, 0x4};
    TEST_CASE(mdi, {0x7c01, 0xffdb, 0x9809}, 0xfffe};

    /*
        TEST_CASE(and, {0x7c01, 
        TEST_CASE(bor, {0x7c01, 
        TEST_CASE(xor, {0x7c01, 
        TEST_CASE(shr, {0x7c01, 
        TEST_CASE(asr, {0x7c01, 
        TEST_CASE(shl, {0x7c01, 
        TEST_CASE(ifb, {0x7c01, 
        TEST_CASE(ifc, {0x7c01, 
        TEST_CASE(ife, {0x7c01, 
        TEST_CASE(ifn, {0x7c01, 
        TEST_CASE(ifg, {0x7c01, 
        TEST_CASE(ifa, {0x7c01, 
        TEST_CASE(ifl, {0x7c01, 
        TEST_CASE(ifu, {0x7c01, 
        TEST_CASE(adx, {0x7c01, 
        TEST_CASE(sbx, {0x7c01, 
        TEST_CASE(sti, {0x7c01, 
        TEST_CASE(std, {0x7c01, 
    */
}
