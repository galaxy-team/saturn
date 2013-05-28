#ifndef __TEST_DEFAULT_CPP
#define __TEST_DEFAULT_CPP

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

#endif
