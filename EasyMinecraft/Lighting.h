#pragma once
#include <string>
#include <iostream>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Shader.h"

class pointLight
{
public:
	pointLight();
	~pointLight();
	void set(glm::vec3 position, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);
	void apply();
private:
	int lightID;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};


class directionLight
{
public:
	void set(glm::vec3 direction, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor);
	void apply();
private:
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class spotLight
{
public:
	spotLight();
	~spotLight(); 
	void spotLight::set(glm::vec3 position, glm::vec3 direction, GLfloat cutOff, GLfloat outerCutOff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
	void apply();
private:
	int lightID;
	float cutOff;
	float outerCutOff;
	glm::vec3 direction;
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};