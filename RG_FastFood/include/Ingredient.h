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
    IngredientType GetType() const { return m_type; }
    void Show() { m_visible = true; }
    void Hide() { m_visible = false; }
    bool IsVisible() const { return m_visible; }

    float GetTipX() const {
        return m_x;
    }

    float GetTipY() const {
        return m_y - m_height * 0.35f;
    }

private:
    IngredientType m_type;
    GLuint m_texture;

    float m_x, m_y;
    float m_speed = 1.0f;
    float m_height = 0.6f;

    bool m_placed = false;
    bool m_visible = true;
};
