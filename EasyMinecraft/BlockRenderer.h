#pragma once
//std headers
#include <vector>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <fstream>
#include <atomic>

//gl headers
#include <glad\glad.h>
#include <GLFW\glfw3.h>

#include <glm\glm.hpp>

#include "Debug.h"
#include "Texture.h"
#include "Camera.h"
#include "Noise.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;
};

//data that will be combined to vertices
const glm::vec3 blockPositions[] =
{
	glm::vec3(-0.5, 0.5, 0.5),
	glm::vec3(-0.5,-0.5, 0.5),
	glm::vec3( 0.5,-0.5, 0.5),
	glm::vec3( 0.5, 0.5, 0.5),
	glm::vec3(-0.5, 0.5,-0.5),
	glm::vec3(-0.5,-0.5,-0.5),
	glm::vec3( 0.5,-0.5,-0.5),
	glm::vec3( 0.5, 0.5,-0.5)
};

const unsigned short blockIndices[][6] =
{
	//every face with two triangles
	//right
	3,2,6,
	6,7,3,
	//left
	4,5,1,
	1,0,4,
	//up
	4,0,3,
	3,7,4,
	//down
	1,5,6,
	6,2,1,
	//front
	0,1,2,
	2,3,0,
	//back
	7,6,5,
	5,4,7
};

const glm::vec3 blockNormals[] =
{
	glm::vec3(1, 0, 0),
	glm::vec3(-1, 0, 0),
	glm::vec3(0, 1, 0),
	glm::vec3(0,-1, 0),
	glm::vec3(0, 0, 1),
	glm::vec3(0, 0,-1)
};

const glm::vec2 blockTexCoords[6] =
{
	//y-value has been inversed for adapting openGL texcord system
	glm::vec2(0,0),
	glm::vec2(0,1),
	glm::vec2(1,1),
	glm::vec2(1,1),
	glm::vec2(1,0),
	glm::vec2(0,0)
};

//block-scale definition
typedef unsigned short blockPos;
const blockPos chunkSizeX = 16, chunkSizeY = 128, chunkSizeZ = 16;
typedef unsigned short blockType;
const blockType blockTypeNum = 5;

//chunk-scale definition
typedef int chunkPos;
typedef glm::ivec2 chunkPosVec;
const chunkPos renderSize = 2;

//world-scale definition
typedef long long globalBlockPos;


enum blockType_enum
{
	AIR,
	DIRT,
	COBBLESTONE,
	GRASS,
	BEDROCK
};

//init renderer
void initBlockRenderer();

class Chunk
{
public:
	Chunk(chunkPosVec position);
	~Chunk();

	void generate(globalBlockPos noiseX, globalBlockPos noiseZ);
	void draw();
	void update();
	void save();
	bool load();
	void debug();
	chunkPos chunkX, chunkZ;
	bool upadated;
private:
	blockType data[chunkSizeX][chunkSizeY][chunkSizeZ];

	//split vertices by different texture
	std::vector<Vertex> verticesBuffer[blockTypeNum];

	friend blockType getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z, const Chunk* currentChunk);
	friend blockType getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z);
	//rendering stuffs
	GLuint vao, vbo, debugVao, debugVbo;
	size_t verticesOffset[blockTypeNum + 1];//offset between vertices
	void addBlockVertices(const blockPos & x, const blockPos & y, const blockPos & z, std::vector<Vertex> verticesGroups[]);
};


//render chunks
void drawWorld();
void drawWorldDebug();
void enableUpdateThread();
void disableUpdateThread();
void updateWorldLoop();

void loadChunk(chunkPosVec position);
blockType getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z, const Chunk* currentChunk);