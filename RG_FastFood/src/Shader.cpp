#include "../include/Shader.h"
#include "../include/Util.h"
#include <iostream>


Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
	m_program = createShader(vertPath.c_str(), fragPath.c_str());
	if (m_program == 0) std::cerr << "Shader creation failed for " << vertPath << " / " << fragPath << std::endl;
}


Shader::~Shader() {
	if (m_program) glDeleteProgram(m_program);
}