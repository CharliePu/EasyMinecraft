#pragma once
#include <iostream>
#include <string>

void sendError(const std::string & description);

void glfwErrorCallback(int error, const char * description);
