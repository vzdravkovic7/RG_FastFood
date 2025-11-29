#include "../include/Renderer.h"
#include "../include/Util.h"
#include "../include/Gameplay.h"
#include <iostream>


Renderer::Renderer(int width, int height) : m_width(width), m_height(height) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	m_shaderProgram = createShader("shaders/rect.vert", "shaders/rect.frag");
	if (m_shaderProgram == 0) std::cerr << "Failed to create shader program\n";


	glUseProgram(m_shaderProgram);
	GLint locTex = glGetUniformLocation(m_shaderProgram, "uTex");
	GLint locTex1 = glGetUniformLocation(m_shaderProgram, "uTex1");
	if (locTex >= 0) glUniform1i(locTex, 0);
	if (locTex1 >= 0) glUniform1i(locTex1, 1);


	SetupQuad();
}


Renderer::~Renderer() {
	if (m_vbo) glDeleteBuffers(1, &m_vbo);
	if (m_vao) glDeleteVertexArrays(1, &m_vao);
	if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
}


void Renderer::SetupQuad() {
	float vertices[] = {
	-0.2f, 0.2f, 0.0f, 1.0f,
	-0.2f, -0.2f, 0.0f, 0.0f,
	0.2f, -0.2f, 1.0f, 0.0f,
	0.2f, 0.2f, 1.0f, 1.0f
	};
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}


void Renderer::Render(Gameplay* gameplay) {
	glClearColor(0.2f, 0.4f, 0.65f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(m_shaderProgram);
	glActiveTexture(GL_TEXTURE0);


	// Delegate drawing choices to Gameplay (it will call glUniforms and draw the VAO)
	gameplay->OnRender(m_shaderProgram, m_vao);
}