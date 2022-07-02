#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <spdlog/spdlog.h>

#define GLCall(x) x

void GLClearError();
bool GLLogCall(const char* function, const char* file, const int& line);
