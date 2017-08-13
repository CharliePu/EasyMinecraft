#include "Debug.h"

void sendError(const std::string &description)
{
	std::cerr << description << std::endl;
}

void glfwErrorCallback(int error, const char * description)
{
	std::cerr << error << ":" << description << std::endl;
}