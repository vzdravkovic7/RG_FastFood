#pragma once
#include <GL/glew.h>
#include <string>


class Texture {
public:
	static GLuint FromFile(const std::string& path);
};