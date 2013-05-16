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
  //        virtual const int get_block_size() = 0;
//          virtual std::array<std::uint16_t, 0> get_block_image() = 0;
        public:
            const static int BLOCK_SIZE = BlockSize;

//          void write_out_image(std::array<uint16_t, BLOCK_SIZE> image);
            void read_in_image(char* file_data_array, int image_filesize);
        };
    }
}

#endif
