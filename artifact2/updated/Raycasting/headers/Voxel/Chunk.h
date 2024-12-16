#pragma once
#include <stdexcept>
#include "Block.h"


class Chunk {
	struct ChunkSize; 

public:
	struct ChunkSize {
		int width, height, depth;
	};
	

	/**
	*
	*
	* Will create a chunk with standard size
	**/
	Chunk() : width(STANDARD.width), height(STANDARD.height), depth(STANDARD.depth), size(STANDARD) {
		if (width < 0 || height < 0 || depth < 0) {
			throw std::runtime_error("Invalid dimensions for chunk");
		}
		blocks = new uint8_t[width * depth * height];

		init();

	}


	Chunk(ChunkSize chunkSize) : width(chunkSize.width), height(chunkSize.height), depth(chunkSize.depth), size(ChunkSize{width, height, depth}) {
		if (width < 0 || height < 0 || depth < 0) {
			throw std::runtime_error("Invalid dimensions for chunk");
		}
		blocks = new uint8_t[width * depth * height];

		init();

	}


	/**
	*
	* w: width (x), h:heigth (y), d:depth (z)
	**/
	Chunk( int w, int h, int d): width(w), height(h), depth(d), size(ChunkSize{ w, h, d }) {
		if (width < 0 || height < 0 || depth < 0) {
			throw std::runtime_error("Invalid dimensions for chunk");
		}
		blocks = new uint8_t[width * depth * height];
		
		init();

	}


	void setBlock(Block b, int x, int y, int z) {
		if (x < 0 || y < 0 || z < 0 || x >= width || y >= height || z >= depth) {
			throw std::runtime_error("Block coordinates outside of chunk bounds");
		}
		int index = x + (width * y) + (width * height * z);

		blocks[index] = b.type; // just copy over values

	}

	/**
	* Sets the vertical column of a chuck to be of block type passed,
	* starting at the height, startY, and going downwards towards bottom of 
	* chuck 0. 
	* 
	* Works well with Height Maps as default values for chuck is 0, or Air block
	**/
	void setVerticalColumn(Block b, int x, int z, int startY) {
		if (x < 0 || startY < 0 || z < 0 || x >= width || startY >= height || z >= depth) {
			throw std::runtime_error("Vertical column outside of chunk bounds");
		}

		int y = startY; 
		int index = x + (width * y) + (width * height * z);
		// can remove the y, and just check if index is >= 0
		while (y >= 0) {
			blocks[index] = b.type;
			index -= width; // move down to next row
			y--; 
		}

	}
	

	uint8_t getBlock(int x, int y, int z) const{
		if (x < 0 || y < 0 || z < 0 || x >= width || y >= height || z >= depth) {
			throw std::runtime_error("Block coordinates outside of chunk bounds");
		}
		return blocks[x + (width * y) + (width * height * z)]; 
	}

	// no checks
	uint8_t getBlockFast(int x, int y, int z) const {
		return blocks[x + (width * y) + (width * height * z)];
	}

	int getWidth() const{
		return width;
	}

	int getHeight() const{
		return height;
	}

	int getDepth() const{
		return depth; 
	}

	const uint8_t* getBlockData() const {
		return blocks;
	}

	void printContents() const {
		for (int z = 0; z < depth; ++z) {
			std::cout << "Layer " << z << ":\n";
			for (int y = 0; y < height; ++y) {
				for (int x = 0; x < width; ++x) {
					std::cout << static_cast<int>(getBlockFast(x, y, z)) << " ";
				}
				std::cout << "\n";
			}
			std::cout << "\n";
		}
	}

	/*
	Because we assign values by vertical column, up to a specific height
	if a value was previously set past the current height, it will
	still be set as an occupied block
	*/
	void clear() {
		for (int i = 0; i < width * height * depth; i++) {
			blocks[i] = 0;
		}
	}

	ChunkSize getSize() {
		return size; // return copy not a reference
	}

private:
	

	const int width, height, depth; // temporary values (x, y, z)
	uint8_t* blocks; // represents 3d chunk as flattened array, (row is y, column is x, z is depth)
	const static inline ChunkSize STANDARD{ 16, 32, 16 };
	
	ChunkSize size; 

	void init() {
		int total = width * height * depth;
		for (int i = 0; i < total; i++) {
			blocks[i] = { 0 }; // assign all to air blocks 
		}
	}



};