#pragma once

#include <fstream>
#include <string>
#include <iostream>

inline std::string readFile(std::string _filename) {
	// open up a file, read from the end
	std::ifstream file(_filename, std::ios::ate | std::ios::binary);
	// if couldn't open - error out
	if(!file.good()) {
		std::cerr << "Couldn't open file: " << _filename << " !\n";
		_ASSERT(false);
	}

	// create a string of the exact size the file is (tellg tells us the position, which is at the end)
	size_t fileSize = file.tellg();
	std::string loaded(fileSize, 0);
	// go to the first position (the file will now be read from the start)
	file.seekg(0);

	// we read it from start to finish, to the "loaded" string
	file.read(loaded.data(), fileSize);

	return loaded;
}