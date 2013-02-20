#ifndef TEST_DEVICE_HPP
#define TEST_DEVICE_HPP

#include <libsaturn.hpp>

class test_device : public galaxy::saturn::device {
    public:
        int count_cycles;
        int count_interrupts;

        test_device() : count_cycles(0), count_interrupts(0) {}

        virtual void cycle();
        virtual void interrupt();
};

#endif // TEST_DEVICE_HPP
