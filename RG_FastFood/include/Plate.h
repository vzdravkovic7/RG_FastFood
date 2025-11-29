#pragma once
#include "Texture.h"
#include "Renderer.h"
#include <GL/glew.h>

class Plate {
public:
    Plate() = default;

    void Init(GLuint texID, float x, float y, float scale) {
        m_tex = texID;
        m_x = x;
        m_y = y;
        m_scale = scale;
    }

    void Render(GLuint shaderProgram, GLuint vao) {
        if (m_tex == 0) return;

        GLint locPos = glGetUniformLocation(shaderProgram, "uPos");
        GLint locScale = glGetUniformLocation(shaderProgram, "uScale");

        glBindVertexArray(vao);
        glBindTexture(GL_TEXTURE_2D, m_tex);
        glUniform2f(locPos, m_x, m_y);
        glUniform2f(locScale, m_scale, m_scale);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

private:
    GLuint m_tex = 0;
    float m_x = 0.0f, m_y = 0.0f;
    float m_scale = 1.0f;
};
