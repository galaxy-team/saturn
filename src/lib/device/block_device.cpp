#include <sstream>
#include <iostream>

void galaxy::saturn::block_device.read_in_image(char* file_data_array, int image_filesize){            
    // read from the buffer into the floppy disk data array
    for (int i = 0; i < (size / 2) && i < image_filesize; i++) {
        block_image[i] = (buffer[i * 2]) << 0x8;
        block_image[i] ^= buffer[i * 2 + 1] & 0xff;
    }

}

