#include "BlockRenderer.h"

GLuint blockTexture[blockTypeNum];

void initBlockRenderer()
{
	//load blocks' textures
	blockTexture[DIRT] = Texture("blocks/dirt.bmp", texture_diffuse_map);
	blockTexture[COBBLESTONE] = Texture("blocks/cobblestone.bmp", texture_diffuse_map);
	blockTexture[GRASS] = Texture("blocks/grass.bmp", texture_diffuse_map);
	blockTexture[BEDROCK] = Texture("blocks/bedrock.bmp", texture_diffuse_map);
}

Chunk::Chunk(chunkPosVec position)
{
	this->chunkX = position.x;
	this->chunkZ = position.y;
	this->upadated = false;

	this->vao = NULL;
	this->vbo = NULL;

	this->debugVao = NULL;
	this->debugVbo = NULL;
}

Chunk::~Chunk()
{
	verticesBuffer->~vector();
	glDeleteVertexArrays(1, &vao);
	glDeleteVertexArrays(1, &debugVao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &debugVbo);
}

void Chunk::generate(globalBlockPos noiseX, globalBlockPos noiseZ)
{
	for (blockPos x = 0; x < chunkSizeX; x++)
		for (blockPos y = 0; y < chunkSizeY; y++)
			for (blockPos z = 0; z < chunkSizeZ; z++)
			{
				data[x][y][z] = AIR;
			}

	for (blockPos x = 0; x < chunkSizeX; x++)
		for (blockPos z = 0; z < chunkSizeZ; z++)
		{
			for (blockPos y = 0; y < mapValue(-2, 2, 10, 30, perlinNoise2D((noiseX+x)/(double)20,(noiseZ+z)/(double)20)); y++)
			{
				data[x][y][z] = COBBLESTONE;
			}
		}
}

void Chunk::draw()
{
	if (this->upadated)
	{
		if (!this->vao)
		{
			glGenVertexArrays(1, &this->vao);
			glGenBuffers(1, &this->vbo);
		}

		glBindVertexArray(this->vao);
		//bind and upload vertices data
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, verticesBuffer[AIR].size() * sizeof(Vertex), &verticesBuffer[AIR][0], GL_STATIC_DRAW);
		//set position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
		//set normal
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, normal));
		//set texCoords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texCoord));
		glBindVertexArray(NULL);

		this->upadated = false;
	}
	glBindVertexArray(this->vao);
	//air is ignored
	for (blockType i = 1; i < blockTypeNum; i++)
	{
		glBindTexture(GL_TEXTURE_2D, blockTexture[i]);
		glDrawArrays(GL_TRIANGLES, verticesOffset[i], verticesOffset[i + 1] - verticesOffset[i]);
	}
	glBindVertexArray(NULL);
}

void Chunk::update()
{
	for (blockType i = 0; i < blockTypeNum; i++)
		verticesBuffer[i].clear();

	//add block vertices
	for (blockPos x = 0; x < chunkSizeX; x++)
		for (blockPos y = 0; y < chunkSizeY; y++)
			for (blockPos z = 0; z < chunkSizeZ; z++)
			{
				addBlockVertices(x, y, z, verticesBuffer);
			}

	//combine them (ignore air)
	for (blockType i = 1; i < blockTypeNum; i++)
	{
		verticesBuffer[AIR].insert(verticesBuffer[AIR].end(), verticesBuffer[i].begin(), verticesBuffer[i].end());
		verticesOffset[i + 1] = verticesBuffer[AIR].size();
	}

	this->upadated = true;
}

void Chunk::save()
{
	std::ofstream chunkFile;
	chunkFile.open("Saves/chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::out | std::ios::trunc | std::ios::binary);
	if (chunkFile.good())
	{
		for (const auto &i : this->data)
			for (const auto &j : i)
				for (const auto &k : j)
				{
					chunkFile.write((char*)&k, sizeof(blockType));
				}
	}
	chunkFile.close();
}

bool Chunk::load()
{
	std::ifstream chunkFile;
	chunkFile.open("Saves/chunk" + std::to_string(chunkX) + "," + std::to_string(chunkZ) + ".dat", std::ios::in | std::ios::binary);
	if (!chunkFile.good())
	{
		chunkFile.close();
		return false;
	}

	for (auto &i : this->data)
		for (auto &j : i)
			for (auto &k : j)
				chunkFile.read((char*)&k, sizeof(blockType));

	chunkFile.close();
	return true;
}

void Chunk::debug()
{
	if (!this->debugVao)
	{
		GLfloat debugFramework[] =
		{
			(this->chunkX + 1) * chunkSizeX - 0.5f,-0.5f			,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,
			this->chunkX * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			(this->chunkX + 1) * chunkSizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,-0.5f			,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			this->chunkX * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,-0.5f			,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,-0.5f			,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,-0.5f			,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			this->chunkX * chunkSizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			(this->chunkX + 1) * chunkSizeX - 0.5f,-0.5f		,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,-0.5f			,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			this->chunkX * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			this->chunkX * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1,

			(this->chunkX + 1) * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,this->chunkZ * chunkSizeZ - 0.5f		,1,1,1,
			(this->chunkX + 1) * chunkSizeX - 0.5f,chunkSizeY - 0.5f	,(this->chunkZ + 1) * chunkSizeZ - 0.5f,1,1,1
		};
		glGenVertexArrays(1, &this->debugVao);
		glGenBuffers(1, &this->debugVbo);
		glBindVertexArray(this->debugVao);
		//bind and upload vertices data
		glBindBuffer(GL_ARRAY_BUFFER, this->debugVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(debugFramework), debugFramework, GL_STATIC_DRAW);
		//set position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
		//set color
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(sizeof(GLfloat) * 3));
		glBindVertexArray(NULL);
	}
	glBindVertexArray(this->debugVao);
	glDrawArrays(GL_LINES, 0, 24);
	glBindVertexArray(NULL);
}

void Chunk::addBlockVertices(const blockPos &x, const blockPos &y, const blockPos &z, std::vector<Vertex> verticesGroups[])
{
	//do nothing if the block is air
	if (data[x][y][z] == AIR)
	{
		return;
	}
	//cull adjacent faces
	//each face ordered in anti-clockwise
	
	Vertex temp;

	//right
	if (x < chunkSizeX)
	{
		if (getBlockType(x + 1, y, z, this) == AIR)
		{
			temp.normal = blockNormals[0];
			for (auto i = 0; i < 6; i++)
			{
				
				temp.texCoord = blockTexCoords[i];
				temp.position = blockPositions[blockIndices[0][i]] + glm::vec3(x + chunkX*chunkSizeX, y, z + chunkZ*chunkSizeZ);
				verticesGroups[data[x][y][z]].push_back(temp);
			}
		}
	}
	//left
	if (x >= 0)
		if (getBlockType(x - 1, y, z, this) == AIR)
		{
			temp.normal = blockNormals[1];
			for (auto i = 0; i < 6; i++)
			{
				temp.position = blockPositions[blockIndices[1][i]] + glm::vec3(x + chunkX*chunkSizeX, y, z + chunkZ*chunkSizeZ);
				temp.texCoord = blockTexCoords[i];
				verticesGroups[data[x][y][z]].push_back(temp);
			}
		}
	//up
	if (y < chunkSizeY - 1)
		if (getBlockType(x, y + 1, z, this) == AIR)
		{
			temp.normal = blockNormals[2];
			for (auto i = 0; i < 6; i++)
			{
				temp.position = blockPositions[blockIndices[2][i]] + glm::vec3(x + chunkX*chunkSizeX, y, z + chunkZ*chunkSizeZ);
				temp.texCoord = blockTexCoords[i];
				verticesGroups[data[x][y][z]].push_back(temp);
			}
		}
	//down
	if (y > 0)
		if (getBlockType(x, y - 1, z, this) == AIR)
		{
			temp.normal = blockNormals[3];
			for (auto i = 0; i < 6; i++)
			{
				temp.position = blockPositions[blockIndices[3][i]] + glm::vec3(x + chunkX*chunkSizeX, y, z + chunkZ*chunkSizeZ);
				temp.texCoord = blockTexCoords[i];
				verticesGroups[data[x][y][z]].push_back(temp);
			}
		}
	//front
	if (z < chunkSizeY)
		if (getBlockType(x, y, z + 1, this) == AIR)
		{
			temp.normal = blockNormals[4];
			for (auto i = 0; i < 6; i++)
			{
				temp.position = blockPositions[blockIndices[4][i]] + glm::vec3(x + chunkX*chunkSizeX, y, z + chunkZ*chunkSizeZ);
				temp.texCoord = blockTexCoords[i];
				verticesGroups[data[x][y][z]].push_back(temp);
			}
		}
	//back
	if (z >= 0)
		if (getBlockType(x, y, z - 1, this) == AIR)
		{
			temp.normal = blockNormals[5];
			for (auto i = 0; i < 6; i++)
			{
				temp.position = blockPositions[blockIndices[5][i]] + glm::vec3(x + chunkX*chunkSizeX, y, z + chunkZ*chunkSizeZ);
				temp.texCoord = blockTexCoords[i];
				verticesGroups[data[x][y][z]].push_back(temp);
			}
		}
}

struct KeyHasher
{
	std::size_t operator()(const chunkPosVec& key) const
	{
		using std::size_t;
		using std::hash;

		return ((key.x * 5209) ^ (key.y * 1811));
	}
};

std::unordered_map <chunkPosVec, Chunk*, KeyHasher> chunkMap;

std::vector<Chunk*> *frontDrawBuffer;
std::vector<Chunk*>	*backDrawBuffer;
std::mutex updateLock;

void drawWorld()
{
	for (auto i : *frontDrawBuffer)
	{
		i->draw();
	}
}

void drawWorldDebug()
{
	for (auto i : *frontDrawBuffer)
	{
		i->debug();
	}
}

std::atomic<bool> updateThreadShouldClose;
std::thread updateThread;
void enableUpdateThread()
{
	updateThreadShouldClose = false;
	updateThread = std::thread(updateWorldLoop);
	updateThread.detach();
}

void disableUpdateThread()
{
	updateThreadShouldClose = true;
	updateLock.lock();
	updateLock.unlock();
}

void updateWorldLoop()
{
	frontDrawBuffer = new std::vector<Chunk*>;
	backDrawBuffer = new std::vector<Chunk*>;
	
	chunkPos currentRenderSize = 0;

	while (!updateThreadShouldClose)
	{
		//clear back draw buffer
		backDrawBuffer->clear();

		//increase load radius gradually
		if (currentRenderSize < renderSize)
			currentRenderSize++;

		//load chunks around current position within load radius
		chunkPosVec currentPosition(getCameraPosition().x / chunkSizeX, getCameraPosition().z / chunkSizeZ);
		for (chunkPos i = 0; i <= currentRenderSize; i++)
			for (chunkPos j = 0; j <= currentRenderSize; j++)
			{
				loadChunk(chunkPosVec(currentPosition) - currentRenderSize / 2 + chunkPosVec(i, j));
			}

		//update chunks that needed
		for (auto i : *backDrawBuffer)
		{
			if (!i->upadated)
			{
				i->update();
			}
		}


		updateLock.lock();
		//swap two buffer of chunks
		std::swap(frontDrawBuffer, backDrawBuffer);

		//unload chunks
		for (auto i : *backDrawBuffer)
		{
			if ((abs(i->chunkX - currentPosition.x) > renderSize/2) || (abs(i->chunkZ - currentPosition.y) > renderSize/2))
			{
				i->save();
				chunkMap.erase(chunkPosVec(i->chunkX, i->chunkZ));
				delete i;
			}
		}
		updateLock.unlock();
	}
	updateLock.lock();
	for (auto i : chunkMap)
	{
		i.second->save();
		delete i.second;
	}
	delete frontDrawBuffer;
	delete backDrawBuffer;
	updateLock.unlock();
}

void loadChunk(chunkPosVec position)
{
	if (!chunkMap[position])
	{
		updateLock.lock();
		chunkMap[position] = new Chunk(position);
		if (!chunkMap[position]->load())
			chunkMap[position]->generate(position.x*chunkSizeX, position.y*chunkSizeZ);
		updateLock.unlock();
	}
	backDrawBuffer->push_back(chunkMap[position]);
}

blockType getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z, const Chunk* currentChunk)
{
	if (!currentChunk)
	{
		sendError("getBlockType: nullptr receieved");
		return AIR;
	}
	if (y >= chunkSizeY || y < 0)
	{
		sendError("getBlockType: abnormal y-value receieved");
		return AIR;
	}
	
	blockPos localX, localY, localZ;
	chunkPos offSetchunkX, offSetChunkZ;

	localY = (blockPos)y;

	localX = (blockPos)(x % chunkSizeX);
	offSetchunkX = (chunkPos)(x / chunkSizeX);
	if (x < 0)
	{
		localX += chunkSizeX;
		offSetchunkX--;
	}

	localZ = (blockPos)(z % chunkSizeZ);
	offSetChunkZ = (chunkPos)(z / chunkSizeZ);
	if (z < 0)
	{
		localZ += chunkSizeZ;
		offSetChunkZ--;
	}

	if (chunkMap.find(chunkPosVec(offSetchunkX + currentChunk->chunkX, offSetChunkZ + currentChunk->chunkZ))==chunkMap.end())
	{
		//sendError("getBlockType: positioned chunk invalid");
		return AIR;
	}

	return chunkMap[chunkPosVec(offSetchunkX + currentChunk->chunkX, offSetChunkZ + currentChunk->chunkZ)]->data[localX][localY][localZ];
}

blockType getBlockType(const globalBlockPos &x, const globalBlockPos &y, const globalBlockPos &z)
{
	if (y >= chunkSizeY || y < 0)
	{
		sendError("getBlockType: abnormal y-value receieved");
		return AIR;
	}

	blockPos localX, localY, localZ;
	chunkPos localChunkX, localChunkZ;

	localY = (blockPos)y;

	localX = (blockPos)(x % chunkSizeX);
	localChunkX = (chunkPos)(x / chunkSizeX);
	if (x < 0)
	{
		localX += chunkSizeX;
		localChunkX--;
	}

	localZ = (blockPos)(z % chunkSizeZ);
	localChunkZ = (chunkPos)(z / chunkSizeZ);
	if (z < 0)
	{
		localZ += chunkSizeZ;
		localChunkZ--;
	}

	if (chunkMap.find(chunkPosVec(localChunkX, localChunkZ)) == chunkMap.end())
	{
		//sendError("getBlockType: positioned chunk invalid");
		return AIR;
	}

	return chunkMap[chunkPosVec(localChunkX, localChunkZ)]->data[localX][localY][localZ];
}
