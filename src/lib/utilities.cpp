#include <sstream>
#include <iostream>
#include <array>
#include <utilities.hpp>
#include <fstream>

#ifndef __UTILITIES_CPP
#define __UTILITIES_CPP

bool galaxy::saturn::utilities::is_read_only(std::string filename){
	std::ifstream file_obj_in;
	file_obj_in.open(filename, std::ios::in);

	if (!file_obj_in.is_open()) {
	   return false;
	}

	file_obj_in.close();

	std::ofstream file_obj_out;
	file_obj_out.open(filename);

	if (!file_obj_in.is_open()) {
	    return true;
	} else {
	    return false;
	}
}

std::vector<std::uint16_t> galaxy::saturn::utilities::string_to_vector(char * string) {
    
}

#endif
