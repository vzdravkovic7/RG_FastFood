#include "../include/AssemblingController.h"
#include "../include/Texture.h"
#include "../include/Input.h"
#include <iostream>

AssemblingController::AssemblingController() {}

void AssemblingController::LoadTextures() {
    m_texBunBottom = Texture::FromFile("res/bottom_bun.png");
    m_texPattie = Texture::FromFile("res/pattie.png");
    m_texKetchup = Texture::FromFile("res/ketchup_bottle.png");
    m_texMustard = Texture::FromFile("res/mustard_bottle.png");
    m_texPickles = Texture::FromFile("res/pickles.png");
    m_texOnion = Texture::FromFile("res/onion_ring.png");
    m_texLettuce = Texture::FromFile("res/lettuce.png");
    m_texCheese = Texture::FromFile("res/cheese.png");
    m_texTomato = Texture::FromFile("res/tomato.png");
    m_texBunTop = Texture::FromFile("res/top_bun.png");
}

void AssemblingController::Start(GLuint cookedPattieTexture) {
    m_list.clear();

    m_list.emplace_back(ING_BUN_BOTTOM, m_texBunBottom);
    m_list.emplace_back(ING_PATTIE, cookedPattieTexture);
    m_list.emplace_back(ING_KETCHUP, m_texKetchup);
    m_list.emplace_back(ING_MUSTARD, m_texMustard);
    m_list.emplace_back(ING_PICKLES, m_texPickles);
    m_list.emplace_back(ING_ONION, m_texOnion);
    m_list.emplace_back(ING_LETTUCE, m_texLettuce);
    m_list.emplace_back(ING_CHEESE, m_texCheese);
    m_list.emplace_back(ING_TOMATO, m_texTomato);
    m_list.emplace_back(ING_BUN_TOP, m_texBunTop);

    m_currentIndex = 0;
    m_done = false;

    // Plate setup
    float plateX = 0.0f;
    float plateY = -0.3f;
    float plateScale = 0.7f;

    GLuint texPlate = Texture::FromFile("res/plate.png");
    m_plate.Init(texPlate, plateX, plateY, plateScale);

    m_stackOffsetY = plateY + 0.02f;
    m_plateInitialized = true;
}

bool AssemblingController::IngredientOverPlate(float x, float y) {
    // plate/table central region where ingredients are "placed"
    return (x > -0.3f && x < 0.3f &&
        y > -0.35f && y < 0.15f);
}

void AssemblingController::Update(float dt) {
    if (m_done) return;

    Ingredient& cur = m_list[m_currentIndex];
    cur.Update(dt);

    // place on SPACE
    if (Input::KeyPressed(GLFW_KEY_SPACE)) {
        if (IngredientOverPlate(cur.GetX(), cur.GetY())) {

            // snaping the ingredient to look good
            cur.ForcePosition(0.0f, m_stackOffsetY);
            cur.MarkPlaced();

            // next ingredient goes a little higher
            m_stackOffsetY += 0.07f;

            m_currentIndex++;

            if (m_currentIndex >= (int)m_list.size()) {
                m_done = true;
            }
        }
    }
}

void AssemblingController::Render(GLuint shaderProgram, GLuint vao) {
    if (m_plateInitialized)
        m_plate.Render(shaderProgram, vao);

    // render all previous placed + current ingredient
    for (int i = 0; i <= m_currentIndex && i < (int)m_list.size(); i++) {
        m_list[i].Render(shaderProgram, vao);
    }
}
