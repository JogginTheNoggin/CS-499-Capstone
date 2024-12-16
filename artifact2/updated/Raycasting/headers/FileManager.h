#pragma once
#define STB_IMAGE_IMPLEMENTATION //need for stb_image to load

#include "stb_image.h"

class FileManager {

public:

	static unsigned char* loadData(const char* path, int* width, int* height, int* numChannels, bool flip) {
		stbi_set_flip_vertically_on_load(flip);
		return stbi_load(path, width, height, numChannels, 0);
	}



private:





};