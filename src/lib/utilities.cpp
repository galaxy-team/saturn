#include <sstream>
#include <iostream>
#include <array>
#include <utilities.hpp>
#include <fstream>
#include <unistd.h>

#ifndef __UTILITIES_CPP
#define __UTILITIES_CPP

bool galaxy::saturn::utilities::is_read_only(std::string filename){
    char const* r_name = filename.c_str();
    return access(r_name, W_OK);
}

std::vector<std::uint16_t> galaxy::saturn::utilities::string_to_vector(char * string) {
    
}

#endif
