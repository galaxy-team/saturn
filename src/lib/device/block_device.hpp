#include <sstream>
#include <iostream>
#include <array>

#ifndef __BLOCK_DEVICE_HPP
#define __BLOCK_DEVICE_HPP

namespace galaxy {
    namespace saturn {
        template <int BlockSize>
        class block_device {
        protected:
        public:
            const static int BLOCK_SIZE = BlockSize;
            std::array<std::uint16_t, BLOCK_SIZE> block_image;

            std::array<uint16_t, BLOCK_SIZE> get_block_image() {
                return block_image;
            }


//            void write_out_image(std::array<uint16_t, BLOCK_SIZE> image);
//            void read_in_image(char* file_data_array, int image_filesize);

            void read_in_image(char* file_data_array, int image_filesize) {
                // BLOCK_SIZE is defined in block_device.hpp
                std::array<std::uint16_t, BLOCK_SIZE> block_image = get_block_image();
                for (int i = 0; i < (image_filesize / 2) && i < BLOCK_SIZE; i++) {
                    block_image[i] = file_data_array[i * 2] << 0x8;
                    block_image[i] ^= file_data_array[i * 2 + 1] & 0xff;
                }
            }
            void write_out_image(std::array<uint16_t, BLOCK_SIZE> image) {};
        };
    }
}

#endif
