#pragma once
#ifndef TEXTURE_H
#define	TEXTURE_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <stdexcept>
#include "FastNoiseLite.h"
#include "FileManager.h"


// Temporary specific class, will make more general, and create noise generator class later
class Texture {
	public:
		unsigned int ID;
		


		enum FORMAT {
			R,
			RG,
			RGB,
			RGBA
		};

		enum DATATYPE {
			BYTE,
			UN_BYTE,
			SHORT,
			UN_SHORT,
			INT,
			UN_INT,
			FLOAT,
			DOUBLE
		};


		Texture(Shader shader, std::string filePath, Texture::DATATYPE dataType ){

			data = FileManager::loadData(filePath.c_str(), &width, &height, &nrChannels, true);

			if (data) {
				std::cout << "Successfully loaded image:" << filePath << ", width:" << width << ", height:" << height << ", channels:" << nrChannels << std::endl;
				
				textFormat = Texture::GL_FORMAT_ARR[Texture::nrChannels - 1]; // set format used in glTexImage2d
				textDataType = Texture::GL_DATA_TYPE_ARR[dataType]; // set data type to be used in glTexImage2d
				
			}
			else {
				std::cout << "Could not load image:" << filePath << std::endl;
				return;
			}
			// stbi_image_free(data); data is used elsewhere, need to keep data for now, though we could use glGetTex

			shaderText = shader; 
			createTexture();
		}


		void bindTexture() {
			glBindTexture(GL_TEXTURE_2D, ID);
		}

		std::string getDefaultUniformName() {
			// std::cout << "uniform name: " << Texture::UNIFROM_PREFIX + std::to_string(textureUnit) << std::endl; 
			return Texture::UNIFROM_PREFIX + std::to_string(textureUnit);
		}

		int getTextureIndex() {
			return textureUnit;
		}

		int getWidth() {
			return width;
		}

		int getHeight() {
			return height;
		}

		bool isReady() {
			return data != nullptr;
		}

		// NOT CORRECT DON'T USE
		void printTextureValues() {
			// Bind the texture
			glBindTexture(GL_TEXTURE_2D, ID);

			// Allocate memory to read the texture data
			float* data = new float[width * height];

			// Read the texture data
			glGetTexImage(GL_TEXTURE_2D, 0, textFormat, textDataType, data);

			// Print the data row by row
			printData(data);

			// Clean up
			delete[] data;

			// Unbind the texture
			// !!!!!!!!! CAUSED uniform texture to have no values , glBindTexture(GL_TEXTURE_2D, 0);
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



	private:
		// for indexing to texture type more dynamically, then perfroming if-else statements 
		inline static const GLint GL_FORMAT_ARR[] = { GL_RED,  GL_RG,  GL_RGB, GL_RGBA };
		inline static const GLenum GL_DATA_TYPE_ARR[] = { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT, GL_FLOAT, GL_DOUBLE };

		inline static const std::string UNIFROM_PREFIX = "texture";

		static const unsigned int maxTextureUnits = 80;
		static inline int nextTextureUnit = -1; // shared amongst instances, each texture
		int textureUnit; 
		int width, height, nrChannels;
		unsigned char* data;
		GLint textFormat;
		GLenum textDataType;
		Shader shaderText;

		/**
		* Get the next avaliable texture unit
		*
		**/
		static unsigned int getNextTextureUnit() {
			if (Texture::nextTextureUnit + 1 >= maxTextureUnits) {
				throw std::runtime_error("Exceeded maximum number of texture units");
			}
			return ++Texture::nextTextureUnit;
		}


		void createTexture() {
			shaderText.use();
			glGenTextures(1, &ID);

			textureUnit = getNextTextureUnit();
			glActiveTexture(GL_TEXTURE0 + textureUnit); 
			glBindTexture(GL_TEXTURE_2D, ID);

			/**/
			// parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Specify the texture format and upload the data
			glTexImage2D(GL_TEXTURE_2D, 0, textFormat, width, height, 0, textFormat, textDataType, data);

			// std::cout << "textureUnit: " << textureUnit << std::endl;

		}



		



		void generateNoiseTexture() {

			float* data = new float[height * width];
			int index = 0;
			FastNoiseLite noise; 
			noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					data[index++] = noise.GetNoise((float)x, (float)y);
				}
			}

			// printData(data);

			glActiveTexture(GL_TEXTURE0); // need to fix so can add textures as i go, throw error if reached max
			glBindTexture(GL_TEXTURE_2D, ID);

			/**/
			// parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Specify the texture format and upload the data
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, data);
			//glGenerateMipmap(GL_TEXTURE_2D);
			
			// Ensure the texture update is complete before it is used in subsequent operations
			glFinish();

			// Clean up
			delete[] data;
			

		}
	

		//old needs to be fixed
		void printData(float* data) {
			// Print data row by row
			for (unsigned int y = 0; y < height; y++) {
				for (unsigned int x = 0; x < width; x++) {
					std::cout << data[y * width + x] << " ";
				}
				std::cout << std::endl;
				std::cout << std::endl;
			}

		}


};



#endif // !TEXTURE_H