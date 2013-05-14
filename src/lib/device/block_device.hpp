#include <sstream>
#include <iostream>

namespace galaxy {
    namespace saturn {
        namespace device {
            class block_device {
            protected:
            public:
                int write_image(std::ofstream file_obj);
                int read_image(std::ifstream file_obj);                
            }
        }
    }
}
