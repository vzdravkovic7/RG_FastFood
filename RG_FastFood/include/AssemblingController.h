#pragma once
#include <vector>
#include "Ingredient.h"
#include "Plate.h"

class AssemblingController {
public:
    AssemblingController();

    void LoadTextures();
    void Start(GLuint cookedPattieTexture);

    void Update(float dt);
    void Render(GLuint shaderProgram, GLuint vao);

    bool IsFinished() const { return m_done; }

private:
    std::vector<Ingredient> m_list;
    int m_currentIndex = 0;
    bool m_done = false;

    // textures for all ingredients
    GLuint m_texBunBottom = 0;
    GLuint m_texPattie = 0;
    GLuint m_texKetchup = 0;
    GLuint m_texMustard = 0;
    GLuint m_texPickles = 0;
    GLuint m_texOnion = 0;
    GLuint m_texLettuce = 0;
    GLuint m_texCheese = 0;
    GLuint m_texTomato = 0;
    GLuint m_texBunTop = 0;

    Plate m_plate;
    float m_stackOffsetY = 0.0f;
    bool m_plateInitialized = false;

    bool IngredientOverPlate(float x, float y);
};
