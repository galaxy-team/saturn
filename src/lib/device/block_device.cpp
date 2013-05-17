#include <sstream>
#include <iostream>

#include "block_device.hpp"

//template <int BlockSize>
template <int BlockSize> void galaxy::saturn::block_device<BlockSize>::read_in_image(char* file_data_array, int image_filesize){
    // BLOCK_SIZE is defined in block_device.hpp, derived from the block_device template
    std::array<std::uint16_t, BLOCK_SIZE> block_image = get_block_image();
    for (int i = 0; i < (image_filesize / 2) && i < BLOCK_SIZE; i++) {
        block_image[i] = file_data_array[i * 2] << 0x8;
        block_image[i] ^= file_data_array[i * 2 + 1] & 0xff;
    }
}

template <int BlockSize> void galaxy::saturn::block_device<BlockSize>::write_out_image(std::array<uint16_t, BLOCK_SIZE> image) {
}

