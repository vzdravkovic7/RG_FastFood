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
    m_texKetchupSpill = Texture::FromFile("res/ketchup_spill.png");
    m_texMustardSpill = Texture::FromFile("res/mustard_spill.png");
}

void AssemblingController::SpawnSpill(GLuint texID, float x, float y, float scale) {
    Spill s;
    s.tex = texID;
    s.x = x;
    s.y = y;
    s.scale = scale;
    m_spills.push_back(s);
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
    float plateY = -0.35f;
    float plateScale = 0.7f;

    GLuint texPlate = Texture::FromFile("res/plate.png");
    m_plate.Init(texPlate, plateX, plateY, plateScale);

    m_stackOffsetY = plateY + 0.02f;
    m_plateInitialized = true;

    for (auto& ing : m_list)
        ing.Hide();

    if (!m_list.empty())
        m_list[0].Show();
}

bool AssemblingController::IngredientOverPlate(float x, float y) {
    return (x > -0.3f && x < 0.3f &&
        y > -0.35f && y < 0.15f);
}

void AssemblingController::Update(float dt) {
    if (m_done) return;

    Ingredient& cur = m_list[m_currentIndex];
    cur.Update(dt);

    bool isSauce = (cur.GetType() == ING_KETCHUP || cur.GetType() == ING_MUSTARD);

    if (Input::KeyPressed(GLFW_KEY_SPACE)) {

        //  SAUCE LOGIC
        if (isSauce)
        {
            float tipX = cur.GetTipX();
            float tipY = cur.GetTipY();

            GLuint spillTex = (cur.GetType() == ING_KETCHUP)
                ? m_texKetchupSpill
                : m_texMustardSpill;

            if (TipOverPlate(tipX, tipY))
            {
                SpawnSpill(spillTex, 0.0f, m_stackOffsetY, 0.22f);

                cur.Hide();
                cur.MarkPlaced();
                m_currentIndex++;

                if (m_currentIndex < (int)m_list.size())
                    m_list[m_currentIndex].Show();
                else
                    m_done = true;

                return;
            }

            if (TipOverTable(tipX, tipY))
            {
                SpawnSpill(spillTex, tipX, tipY, 0.18f);

                cur.Hide();
                cur.MarkPlaced();
                m_currentIndex++;

                if (m_currentIndex < (int)m_list.size())
                    m_list[m_currentIndex].Show();
                else
                    m_done = true;

                return;
            }

            return;
        }

        //  STANDARD INGREDIENT
        if (IngredientOverPlate(cur.GetX(), cur.GetY()))
        {
            cur.ForcePosition(0.0f, m_stackOffsetY);
            cur.MarkPlaced();
            m_stackOffsetY += 0.07f;

            cur.Hide();
            m_currentIndex++;

            if (m_currentIndex < (int)m_list.size())
                m_list[m_currentIndex].Show();
            else
                m_done = true;
        }
    }
}

void AssemblingController::Render(GLuint shaderProgram, GLuint vao) {
    if (m_plateInitialized)
        m_plate.Render(shaderProgram, vao);

    for (const Spill& s : m_spills) {
        if (s.tex == 0) continue;
        glBindTexture(GL_TEXTURE_2D, s.tex);
        GLint locPos = glGetUniformLocation(shaderProgram, "uPos");
        GLint locScale = glGetUniformLocation(shaderProgram, "uScale");
        glUniform2f(locPos, s.x, s.y);
        glUniform2f(locScale, s.scale, s.scale);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    for (int i = 0; i < (int)m_list.size(); i++) {
        if (!m_list[i].IsVisible() && !m_list[i].IsPlaced())
            continue;

        if ((m_list[i].GetType() == ING_KETCHUP || m_list[i].GetType() == ING_MUSTARD) && m_list[i].IsPlaced())
            continue;

        m_list[i].Render(shaderProgram, vao);
    }
}

bool AssemblingController::TipOverPlate(float tipX, float tipY)
{
    return (tipX > -0.25f && tipX < 0.25f &&
        tipY > -0.35f && tipY < -0.10f);
}

bool AssemblingController::TipOverTable(float tipX, float tipY)
{
    return (tipY > -0.9f && tipY < -0.35f);
}
