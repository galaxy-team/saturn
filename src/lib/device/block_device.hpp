#include <sstream>
#include <iostream>

#ifndef __BLOCK_DEVICE_HPP
#define __BLOCK_DEVICE_HPP

namespace galaxy {
    namespace saturn {
        class block_device {
        protected:
        public:
            virtual int BLOCK_SIZE;
            virtual std::array<std::uint16_t, BLOCK_SIZE> block_image;

            void write_out_image(std::array<std::uint16_t, BLOCK_SIZE> image);
            void read_in_image(char* file_data_array[], int image_filesize);
        };
    }
}

#endif
