#ifndef TEST_DEVICE_HPP
#define TEST_DEVICE_HPP

#include <libsaturn.hpp>

class test_device : public galaxy::saturn::device {
    public:
        int count_interrupts;

        test_device() : count_interrupts(0) {}

        void interrupt();
};

#endif // TEST_DEVICE_HPP
