#pragma once
#include "chunk.h"
#include "../FastNoiseLite.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// TODO make dynamic, and changing, with position lookup table
class ChunkManager {



public:
	int startX, endX;
	int startY, endY;
	int startZ, endZ;
	/**
	**
	* Constructor for generating chunks based of noise
	**/
	ChunkManager(int radiusX, int chunkCountY, int radiusZ, glm::vec3 genPos, FastNoiseLite* noise)
		:width(2 * radiusX + 1), height(chunkCountY), depth(2 * radiusZ + 1), size(width* height* depth) {
		if (width < 0 || height < 0 || depth < 0) {
			throw std::runtime_error("Invalid dimensions for Chunk Manager");
		}
		else if (noise == nullptr) {
			throw std::runtime_error("Noise is cannot be NULL");
		}

		this->radiusX = radiusX;
		this->radiusZ = radiusZ; 
		// used to determine how much to add in each direction given an initial position to make chunkCount

		this->noise = noise;

		chunks = new Chunk[size];
		chunkDimen = chunks[0].getSize();


		blockCountX = width * chunkDimen.width;
		blockCountY = height * chunkDimen.height;
		blockCountZ = depth * chunkDimen.depth;

		//int startIndex = 

		// genPos is the initial position where the chucks are generated from
		// block (world) coordinates not, chunk coordinate
		startX = genPos.x - (radiusX * chunkDimen.width);
		endX = genPos.x + ((radiusX + 1) * chunkDimen.width); // don't forget + 1 if, camera at bottom left of chunk
		startZ = genPos.z - ((radiusZ + 1) * chunkDimen.depth);// also needs + 1
		endZ = genPos.z + (radiusZ * chunkDimen.depth);
		startY = 0;
		endY = chunkDimen.height;


		//TODO ignoring the Y, may need if adding chunks on top of one another 
		currentChunk = (width * radiusZ) + radiusX; // remember off by 1 (0 to size - 1), should start in middle chunk

		initChunks();
		create3DTexture();

		update3DTexture();
		
	}
	


	~ChunkManager() {
		delete[] chunks;
	}

	void bind3DTexture() const {
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_3D, textureID);
	}

	int getchunkCountX() const {
		return width;
	}

	int getchunkCountY() const {
		return height;
	}

	int getchunkCountZ() const {
		return depth;
	}

	int getblockCountX() const {
		return blockCountX;
	}

	int getblockCountY() const {
		return blockCountY;
	}

	int getblockCountZ() const {
		return blockCountZ;
	}



	int getCurrentChunk(glm::vec3 pos) {
		int x = (pos.x - startX) / chunkDimen.width;
		int z = (pos.z - startZ) / chunkDimen.depth;

		return x + (width * height * z); // y = 1 we assume flat, as we are only interested in the 

	}

	


	// use chunkIndex to generate chunk instead, when in corner pos is shared
	bool updatePosition(glm::vec3 pos) {

		int chunkIndex = getCurrentChunk(pos);
		// need to know which chunk move to, x+, x-, z-, z+, or the diagonals
		// adjust startX, endX, StartY, EndY based on this
		if (chunkIndex != currentChunk) { // if moved to chunk, generate newset of chunks, needs update to only update relevant portion
			genNewChunks(chunkIndex);
			//printDebug();
			return true;
		}

		return false;
	}

	
	void printDebug() {
		std::cout << "Start chunks: " << startX <<", " << startY << ", " << startZ << std::endl;
		std::cout << "End chunks: " << endX << "," << endY << ", " << endZ << std::endl;
	}
	


private:
	
	const int width, height, depth; // x, y , z, total amount of chunks in a give axis
	int blockCountX, blockCountY, blockCountZ;
	const int size; // total amount of chunks
	Chunk* chunks;
	Chunk::ChunkSize chunkDimen; 
	FastNoiseLite* noise;
	unsigned int textureID;






	//EXPERIMENTATION
	//int lookUpTable[][];
	int radiusX, radiusZ;
	int currentChunk; // position on x,z plane 
	

	/**
	*
	*
	* Need to use chunk index to generate new chunks as position is shared at corners
	* as well as other issues
	*/
	void genNewChunks(glm::vec3 pos) {
		startX = pos.x - (radiusX * chunkDimen.width);
		endX = pos.x + ((radiusX + 1) * chunkDimen.width); // plus one because assumeing position is to be in bottom left corner of current chunk
		startZ = pos.z - ((radiusZ + 1) * chunkDimen.depth); // similar to above
		endZ = pos.z + (radiusZ * chunkDimen.depth);

		currentChunk = (width * radiusZ) + radiusX; // remember off by 1 (0 to size - 1), should start in middle chunk

		

		initChunks();
		//create3DTexture();

		update3DTexture();

	}

	void genNewChunks(int newChunkIndex) {
		int adjX = chunkDimen.width; // how much we adjust the bounds of the chunks
		int adjZ = chunkDimen.depth;
		if (newChunkIndex < currentChunk) { //left of middle chunk
			// left, right-down, down, left-down
			if (newChunkIndex > currentChunk - width) { // greater than down
				if (newChunkIndex == currentChunk - 1) { // left
					adjX *= -1; // move backwards
					adjZ = 0; // no change in z 
				}
				else { // right-down
					// increase x, leave default
					adjZ *= -1; // decrease z
				}
			}
			else { // down, left-down
				if (newChunkIndex == currentChunk - width) { // down
					adjX = 0; // no change x
					adjZ *= -1;
				}
				else { // left down
					adjX *= -1;
					adjZ *= -1;
				}
			}
		}
		else { // right of middle chunk
			// right, left-up, up, right-up
			if (newChunkIndex < currentChunk + width) { // left-up, right 
				if (newChunkIndex == currentChunk + 1) { // right
					// x increase, leave default
					adjZ = 0;// no change in z
				}
				else { // left-up
					adjX *= -1; // decrease X
					// increase Z, leave default
				}
			}
			else { // up, right-up
				if (newChunkIndex == currentChunk + width) {// up
					adjX = 0;// no change in x, 
					// increase z, do not change default
				}
				else { // right up
					// no change in either both increase
				}
			}
		}
		startX += adjX;
		endX += adjX;
		startZ += adjZ;
		endZ += adjZ;



		//currentChunk = (width * radiusZ) + radiusX; // remember off by 1 (0 to size - 1), should start in middle chunk

		initChunks();
		//create3DTexture();
		update3DTexture();
		//debugUpdate();

	}


	// issue generating if chunks not reset
	void generateChunkDetails(Chunk& chunk, int chunkX, int chunkY, int chunkZ) {

		chunk.clear(); // reset chunk before processing *important*



		// use the startX, and startY to adjust the coordinates for noise sampler
		int baseX = (chunkX * chunkDimen.width) + startX; // start x, each chunk has a width, multiple this by the chunkX (chunk position in manager)
		int baseZ = (chunkZ * chunkDimen.depth) + startZ; // start y, similiar to above
		int worldX, worldZ;
		int blockHeight = 0;
		//int index = chunkX + (width * chunkY) + (width * height * chunkZ); debuggging
		//std::cout << "index: " << index << std::endl; 
		float noiseHeight; 
		for (int x = 0; x < chunkDimen.width; x++) {
			
			worldX = baseX + x; // move position (world Coordinate) along the chunk x axis until baseX + width reached
			for (int z = 0; z < chunkDimen.depth; z++) {
				worldZ = baseZ + z; // move the position (world Coordinate) along the chunk z axs up to the depth of the chunk
				noiseHeight = noise->GetNoise(float(worldX), float(worldZ));

				blockHeight = abs(static_cast<int>(noiseHeight * chunkDimen.height)); // convert float to chunk height equivalent, make sure not negative, 1.0 == chunkHeight
				//blockHeight = index; 
				
				chunk.setVerticalColumn({ 1 }, x, z, blockHeight); // Assuming 1 is the block type for solid block
				//chunk.setVerticalColumn({ 1 }, x, z, std::min(blockHeight, 31)); // Assuming 1 is the block type for solid block
			}
		}
	}


	void initChunks() {
		int index = 0;
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				for (int z = 0; z < depth; z++) {
					index = x + (width * y) + (width * height * z); // do not do index++ chatGPT is stupid and creates bugs
					generateChunkDetails(chunks[index], x, y, z);
				}
			}
		}
	}

	void create3DTexture() {
		glGenTextures(1, &textureID);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_3D, textureID);



		int totalWidth = chunkDimen.width * width;
		int totalHeight = chunkDimen.height * height;
		int totalDepth = chunkDimen.depth * depth;

		// Initialize an array with the intended size filled with zeros
		std::vector<uint8_t> initialData(totalWidth * totalHeight * totalDepth, 0);



		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, chunkDimen.width * width, chunkDimen.height * height, chunkDimen.depth * depth, 0, GL_RED, GL_UNSIGNED_BYTE, initialData.data());

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_3D, 0);




	}

	// likely unoptimal, sends chucks one at a time
	// need to be able to update a row or column, as well as an individual chunk
	void update3DTexture() {
		glBindTexture(GL_TEXTURE_3D, textureID);

		// does not work, need to do each x first, 

		
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				for (int z = 0; z < depth; z++) {
					int index = x + (width * y) + (width * height * z);
					glTexSubImage3D(GL_TEXTURE_3D, 0, x * chunkDimen.width, y * chunkDimen.height, z * chunkDimen.depth, 
						chunkDimen.width, chunkDimen.height, chunkDimen.depth, GL_RED, GL_UNSIGNED_BYTE, chunks[index].getBlockData());
				}
			}
		}


		glBindTexture(GL_TEXTURE_3D, 0);
	}


}; 
