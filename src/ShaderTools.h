#pragma once

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <GL/glew.h>

std::string loadShaderCode(const char* filename);

GLuint compileShaderObject(const GLchar *shaderCode, GLenum shaderType);

GLuint ShaderProgram(const char* vsFilename, const char* fsFilename);

