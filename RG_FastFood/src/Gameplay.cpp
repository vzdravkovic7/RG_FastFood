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

    m_assembling.LoadTextures();

    m_texPrijatno = Texture::FromFile("res/prijatno.png");

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
            m_assembling.Start(m_texPattieCooked);
            std::cout << "Cooking finished -> ASSEMBLING\n";
        }
    }

    if (m_state == STATE_ASSEMBLING) {
        m_assembling.Update(dt);

        if (m_assembling.IsFinished()) {
            m_state = STATE_FINISHED;
            std::cout << "Burger completed -> FINISHED" << std::endl;
        }
    }
}

void Gameplay::OnRender(GLuint shaderProgram, GLuint vao)
{
    GLint locAlpha = glGetUniformLocation(shaderProgram, "uAlpha");
    if (locAlpha >= 0) glUniform1f(locAlpha, 1.0f);

    glBindVertexArray(vao);

    GLint locPos = glGetUniformLocation(shaderProgram, "uPos");
    GLint locScale = glGetUniformLocation(shaderProgram, "uScale");
    GLint locUseTex1 = glGetUniformLocation(shaderProgram, "useTex1");
    GLint locBlend = glGetUniformLocation(shaderProgram, "uBlend");

    auto DrawTexSimple = [&](GLuint texID, float x, float y, float scaleX, float scaleY = 0.0f) {
        if (texID == 0) return;
        if (scaleY == 0.0f) scaleY = scaleX;
        glBindTexture(GL_TEXTURE_2D, texID);
        if (locUseTex1 >= 0) glUniform1i(locUseTex1, 0);
        glUniform2f(locPos, x, y);
        glUniform2f(locScale, scaleX, scaleY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        };

    auto DrawTexBlend = [&](GLuint baseTex, float x, float y, float scale, GLuint tex1ID, float blend) {
        if (baseTex == 0) return;
        glBindTexture(GL_TEXTURE_2D, baseTex);
        if (locUseTex1 >= 0) {
            if (tex1ID != 0) {
                glUniform1i(locUseTex1, 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, tex1ID);
                glActiveTexture(GL_TEXTURE0);
                if (locBlend >= 0) glUniform1f(locBlend, blend);
            }
            else {
                glUniform1i(locUseTex1, 0);
            }
        }
        glUniform2f(locPos, x, y);
        glUniform2f(locScale, scale, scale);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        if (locUseTex1 >= 0) glUniform1i(locUseTex1, 0);
        };

    switch (m_state)
    {
    case STATE_START_MENU:
        DrawTexSimple(m_texButton, m_btnPosX, m_btnPosY, m_btnScale);
        break;

    case STATE_COOKING:
    {
        DrawTexSimple(m_texStove, m_stoveCenterX, m_stoveCenterY, 1.5f);

        // Smooth blend
        DrawTexBlend(m_texPattieRaw, m_pattieX, m_pattieY, 0.5f, m_texPattieCooked, m_cookProgress);

        // Cooking bar background (gray)
        DrawTexSimple(m_texGray, m_barX, m_barY, m_barScale);

        // Filled part (green)
        float fillWidth = m_barScale * m_cookProgress;
        float leftEdgeCenter = m_barX - m_barScale * 0.5f;
        float greenCenterX = leftEdgeCenter + fillWidth * 0.5f;
        DrawTexSimple(m_texGreen, greenCenterX, m_barY, fillWidth, m_barScale * 0.9f);

        break;
    }

    case STATE_ASSEMBLING:
        DrawTexSimple(m_texTable, 0.0f, -0.2f, 2.0f);
        m_assembling.Render(shaderProgram, vao);
        break;

    case STATE_FINISHED:
        // Render final burger assembled on plate
        m_assembling.Render(shaderProgram, vao);

        // Render “Prijatno!” texture
        RenderFinalMessage(shaderProgram, vao);
        break;
    }

    // Always show signature in upper-right corner (semi-transparent)
    float sigScaleX = 1.0f;
    float sigScaleY = 1.0f;
    float sigX = 0.75f;
    float sigY = 0.75f;

    if (locAlpha >= 0) glUniform1f(locAlpha, 0.4f);
    DrawTexSimple(m_texSignature, sigX, sigY, sigScaleX, sigScaleY);
    if (locAlpha >= 0) glUniform1f(locAlpha, 1.0f);
}


void Gameplay::RenderFinalMessage(GLuint shaderProgram, GLuint vao)
{
    if (m_texPrijatno == 0) return;

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, m_texPrijatno);

    GLint locPos = glGetUniformLocation(shaderProgram, "uPos");
    GLint locScale = glGetUniformLocation(shaderProgram, "uScale");

    float x = 0.0f;
    float y = 0.65f;
    float sx = 1.5f;
    float sy = 1.5f;

    glUniform2f(locPos, x, y);
    glUniform2f(locScale, sx, sy);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
