#include "../include/Ingredient.h"
#include "../include/Input.h"

Ingredient::Ingredient(IngredientType type, GLuint textureID)
    : m_type(type), m_texture(textureID)
{
    ResetPosition();
}

void Ingredient::ResetPosition() {
    m_x = 0.0f;
    m_y = 0.7f;
}

void Ingredient::Update(float dt) {
    if (m_placed) return;

    if (Input::KeyDown(GLFW_KEY_W)) m_y += m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_S)) m_y -= m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_A)) m_x -= m_speed * dt;
    if (Input::KeyDown(GLFW_KEY_D)) m_x += m_speed * dt;
}

void Ingredient::Render(GLuint shaderProgram, GLuint vao) {
    glBindTexture(GL_TEXTURE_2D, m_texture);

    GLint locPos = glGetUniformLocation(shaderProgram, "uPos");
    GLint locScale = glGetUniformLocation(shaderProgram, "uScale");

    glUniform2f(locPos, m_x, m_y);
    glUniform1f(locScale, 0.6f);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void Ingredient::ForcePosition(float x, float y) {
    m_x = x;
    m_y = y;
}
