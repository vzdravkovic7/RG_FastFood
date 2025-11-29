#include "../include/Gameplay.h"
#include "../include/Texture.h"
#include "../include/Input.h"
#include "../include/Renderer.h"
#include <iostream>
#include <cmath>

Gameplay::Gameplay(Renderer* renderer, GLFWwindow* window)
    : m_renderer(renderer), m_window(window)
{
    // load textures
    m_texButton = Texture::FromFile("res/button_order.png");
    m_texCursor = Texture::FromFile("res/cursor_spatula.png");
    m_texPattieRaw = Texture::FromFile("res/pattie_raw.png");
    m_texPattieCooked = Texture::FromFile("res/pattie.png");
    m_texStove = Texture::FromFile("res/stove.png");
    m_texTable = Texture::FromFile("res/table.png");
    m_texSignature = Texture::FromFile("res/signature.png");
    m_texGreen = Texture::FromFile("res/_solid_green.png");
    m_texGray = Texture::FromFile("res/_solid_gray.png");

    if (m_texCursor) Input::InstallCursor(window, "res/cursor_spatula.png");
}

void Gameplay::Update(float dt)
{
    // handle click once (start menu)
    int leftState = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
    bool leftNow = (leftState == GLFW_PRESS);

    if (leftNow && !m_leftPrev) {
        double mx, my;
        glfwGetCursorPos(m_window, &mx, &my);
        float ndcX = (float)((mx / m_renderer->GetWidth()) * 2.0 - 1.0);
        float ndcY = (float)(-((my / m_renderer->GetHeight()) * 2.0 - 1.0));

        if (m_state == STATE_START_MENU) {
            float bx0 = m_btnPosX - 0.2f * m_btnScale;
            float bx1 = m_btnPosX + 0.2f * m_btnScale;
            float by0 = m_btnPosY - 0.2f * m_btnScale;
            float by1 = m_btnPosY + 0.2f * m_btnScale;

            if (ndcX >= bx0 && ndcX <= bx1 && ndcY >= by0 && ndcY <= by1) {
                m_state = STATE_COOKING;
                m_pattieX = 0.0f;
                m_pattieY = 0.0f;
                m_cookProgress = 0.0f;
                std::cout << "Order button clicked -> COOKING\n";
            }
        }
    }
    m_leftPrev = leftNow;

    // COOKING LOGIC
    if (m_state == STATE_COOKING) {
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) m_pattieY += m_pattieSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) m_pattieY -= m_pattieSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) m_pattieX -= m_pattieSpeed * dt;
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) m_pattieX += m_pattieSpeed * dt;

        bool touchingStove =
            (m_pattieX >= m_stoveCenterX - m_stoveHalfW) &&
            (m_pattieX <= m_stoveCenterX + m_stoveHalfW) &&
            (m_pattieY >= m_stoveCenterY - m_stoveHalfH) &&
            (m_pattieY <= m_stoveCenterY + m_stoveHalfH);

        if (touchingStove) {
            m_cookProgress += dt * m_cookRate;
            if (m_cookProgress > 1.0f) m_cookProgress = 1.0f;
        }

        if (m_cookProgress >= 1.0f) {
            m_state = STATE_ASSEMBLING;
            std::cout << "Cooking finished -> ASSEMBLING\n";
        }
    }
}

void Gameplay::OnRender(GLuint shaderProgram, GLuint vao)
{
    glBindVertexArray(vao);

    GLint locPos = glGetUniformLocation(shaderProgram, "uPos");
    GLint locScale = glGetUniformLocation(shaderProgram, "uScale");
    GLint locUseTex1 = glGetUniformLocation(shaderProgram, "useTex1");
    GLint locBlend = glGetUniformLocation(shaderProgram, "uBlend");

    auto DrawTex = [&](GLuint texID, float x, float y, float scale, GLuint tex1ID = 0, float blend = 0.0f) {
        if (texID == 0) return;
        glBindTexture(GL_TEXTURE_2D, texID);
        if (locUseTex1 >= 0 && locBlend >= 0) {
            if (tex1ID != 0) {
                glUniform1i(locUseTex1, 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, tex1ID);
                glActiveTexture(GL_TEXTURE0);
                glUniform1f(locBlend, blend);
            }
            else {
                glUniform1i(locUseTex1, 0);
            }
        }
        glUniform2f(locPos, x, y);
        glUniform1f(locScale, scale);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        };

    switch (m_state)
    {
    case STATE_START_MENU:
        DrawTex(m_texButton, m_btnPosX, m_btnPosY, m_btnScale);
        break;

    case STATE_COOKING:
    {
        DrawTex(m_texStove, m_stoveCenterX, m_stoveCenterY, 1.5f);

        // Smooth blend
        DrawTex(m_texPattieRaw, m_pattieX, m_pattieY, 0.5f, m_texPattieCooked, m_cookProgress);

        // Cooking bar background
        DrawTex(m_texGray, m_barX, m_barY, m_barScale);

        // Filled part (green)
        float fillScale = m_barScale * m_cookProgress;
        float fillPosX = m_barX - (m_barScale - fillScale) * 0.5f;
        DrawTex(m_texGreen, fillPosX, m_barY, fillScale);

        break;
    }

    case STATE_ASSEMBLING:
        DrawTex(m_texTable, 0.0f, -0.2f, 1.5f);
        break;

    case STATE_FINISHED:
        DrawTex(m_texSignature, 0.0f, 0.0f, 1.0f);
        break;
    }
}
