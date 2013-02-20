#include "test_device.hpp"

void test_device::cycle() {
    count_cycles++;
}

void test_device::interrupt() {
    count_interrupts++;
}
