#pragma once
#include <GL/glew.h>
#include <string>


class Gameplay;


class Renderer {
public:
	Renderer(int width, int height);
	~Renderer();


	void Render(Gameplay* gameplay);


	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }


private:
	void SetupQuad();
	GLuint m_vao = 0, m_vbo = 0;
	GLuint m_shaderProgram = 0;
	int m_width, m_height;
};