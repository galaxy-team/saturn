#include <sstream>
#include <iostream>

void galaxy::saturn
//::device
::block_device.read_image(std::ifstream file_obj){            
    file_obj.seekg(0, std::ios::end);

    image_filesize = file_obj.tellg();

    // i know its not supposed to output anything, this is just for debugging purposes
    std::cout << "Filesize obtained; " << image_filesize << " bytes" << std::endl;

    // create an appropriately sized buffer and read into it
    char* buffer = new char[image_filesize];
    file_obj.read(buffer, image_filesize);

    // read from the buffer into the floppy disk data array
    for (int i = 0; i < (size / 2) && i < image_filesize; i++) {
        block_image[i] = (buffer[i * 2]) << 0x8;
        block_image[i] ^= buffer[i * 2 + 1] & 0xff;
    }

    // wipe the buffer clean, close the file
    delete[] buffer;
}

