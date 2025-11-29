#pragma once
#include <GL/glew.h>
#include <string>


class Shader {
public:
	explicit Shader(const std::string& vertPath, const std::string& fragPath);
	~Shader();
	GLuint GetProgram() const { return m_program; }
private:
	GLuint m_program = 0;
};