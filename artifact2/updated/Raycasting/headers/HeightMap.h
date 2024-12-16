#pragma once
#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <string>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Texture.h"


class HeightMap {
	//TODO need to pass relavent shader or delete redefine class altogether
public:
	unsigned char* data;
	int width, height, nrChannels;
	unsigned int ID;

	HeightMap(std::string filePath) {

        data = FileManager::loadData(filePath.c_str(), &width, &height, &nrChannels, true);

		if (data) {
			std::cout << "Successfully loaded image:" << filePath << ", width:" << width << ", height:" << height << ", channels:" << nrChannels << std::endl;
			//throw std::runtime_error("Height map file could not be opened");
		}
		else {
			std::cout << "Could not load image:" << filePath << std::endl;
			return; 
		}
		// stbi_image_free(data);

		createTexture();
	}

	void print(int amount) {
		if (!data) {
			std::cout << "Data is not loaded." << std::endl;
			return;
		}

		int elementsToPrint = std::min(amount, width * height * nrChannels); // Ensure we don't go out of bounds
		for (int i = 0; i < elementsToPrint; ++i) {
			std::cout << static_cast<int>(data[i]) << " ";
			
		}
		std::cout << std::endl;
	}


	void printMax() {
		if (!data) {
			std::cout << "Data is not loaded." << std::endl;
			return;
		}

		// int elementsToPrint = std::min(1000, width * height * nrChannels); // Ensure we don't go out of bounds
		int elementsToPrint = width * height * nrChannels; // Ensure we don't go out of bounds
		int maxId = 0;
		for (int i = 0; i < elementsToPrint; ++i) {
			if (data[i] > data[maxId]) {
				maxId = i; 
			}
		}
		std::cout << "Max Height: " << static_cast<int>(data[maxId]) << std::endl;
	}

	bool isReady() {
		return data != nullptr;
	}


	
	void createTexture() {


		glGenTextures(1, &ID);


		glActiveTexture(GL_TEXTURE0); // need to fix so can add textures as i go, throw error if reached max
		glBindTexture(GL_TEXTURE_2D, ID);

		/**/
		// parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Specify the texture format and upload the data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

		glBindTexture(GL_TEXTURE_2D, ID);
	}





private:

	 
	





};

#endif HEIGHTMAP_H
