#ifndef TEST_DEVICE_HPP
#define TEST_DEVICE_HPP

#include <device.hpp>

namespace galaxy {
    namespace saturn {
        class test_device : device {
          public:
            int count_interrupts;
            void interrupt();
        };
    }
}

#endif // TEST_DEVICE_HPP
