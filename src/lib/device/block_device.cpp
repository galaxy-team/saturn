#include <sstream>
#include <iostream>

#include "block_device.hpp"

void galaxy::saturn::block_device::read_in_image(char* file_data_array, int image_filesize){            
    // read from the buffer into the floppy disk data array
    for (int i = 0; i < (image_filesize / 2) && i < BLOCK_SIZE; i++) {
        block_image[i] = file_data_array[i * 2] << 0x8;
        block_image[i] ^= file_data_array[i * 2 + 1] & 0xff;
    }

}

