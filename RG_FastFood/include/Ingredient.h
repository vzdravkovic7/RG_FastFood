#pragma once
#include <GL/glew.h>

enum IngredientType {
    ING_BUN_BOTTOM,
    ING_PATTIE,
    ING_KETCHUP,
    ING_MUSTARD,
    ING_PICKLES,
    ING_ONION,
    ING_LETTUCE,
    ING_CHEESE,
    ING_TOMATO,
    ING_BUN_TOP
};

class Ingredient {
public:
    Ingredient(IngredientType type, GLuint textureID);

    void ResetPosition();
    void Update(float dt);
    void Render(GLuint shaderProgram, GLuint vao);

    bool IsPlaced() const { return m_placed; }
    void MarkPlaced() { m_placed = true; }

    void ForcePosition(float x, float y);

    float GetX() const { return m_x; }
    float GetY() const { return m_y; }

private:
    IngredientType m_type;
    GLuint m_texture;

    float m_x, m_y;
    float m_speed = 1.0f;

    bool m_placed = false;
};
