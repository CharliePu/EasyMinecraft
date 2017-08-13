#include "Texture.h"

Texture::Texture(const std::string &path, const textureType &type)
{
	this->type = type;
	//load image
	int width, height;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, NULL, STBI_rgb_alpha);
	if (!data)
	{
		sendError("Failed to load texture " + (std::string)path + ' ' + stbi_failure_reason());
		return;
	}

	//bind texture
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//free data
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const GLuint &id, const textureType &type)
{
	this->id = id;
	this->type = type;
}

Texture::operator GLuint()
{
	return id;
}

Texture::Texture(std::vector<std::string> paths)
{
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
	int width, height;
	unsigned char* data;
	for (GLuint i = 0; i < 6; i++)
	{
		data = stbi_load(paths[i].c_str(), &width, &height, NULL, STBI_rgb_alpha);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		if (!data)
		{
			sendError("Failed to load texture " + (std::string)path + ' ' + stbi_failure_reason());
			continue;
		}
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//y value is inversed, still subTexCoords should inversed y value
glm::vec2 getTextureAtlasCoords(const unsigned short & xNum, const unsigned short & yNum, const unsigned short & subTextureID, const glm::vec2 &subTexCoords)
{
	GLfloat xInterval = 1 / (GLfloat)xNum;
	GLfloat yInterval = 1 / (GLfloat)yNum;
	GLushort yPos = subTextureID / xNum;
	GLushort xPos = subTextureID % xNum;
	//return glm::vec2(subTexCoords.x, subTexCoords.y+yInterval);
	return glm::vec2((subTexCoords.x + xPos)*xInterval, (subTexCoords.y + yPos)*yInterval);
}