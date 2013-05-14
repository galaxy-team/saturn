#include <sstream>
#include <iostream>

namespace galaxy {
    namespace saturn {
        namespace device {
            class block_device {
            protected:
            public:
                void write_image(std::ofstream file_obj);
                void read_image(std::ifstream file_obj);
            }
        }
    }
}
