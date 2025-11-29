#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Renderer;

enum GameState { STATE_START_MENU, STATE_COOKING, STATE_ASSEMBLING, STATE_FINISHED };

class Gameplay {
public:
    Gameplay(Renderer* renderer, GLFWwindow* window);
    void Update(float dt);
    void OnRender(GLuint shaderProgram, GLuint vao);

private:
    Renderer* m_renderer = nullptr;
    GLFWwindow* m_window = nullptr;
    GameState m_state = STATE_START_MENU;

    // resources
    GLuint m_texButton = 0;
    GLuint m_texCursor = 0;
    GLuint m_texPattieRaw = 0;
    GLuint m_texPattieCooked = 0;
    GLuint m_texStove = 0;
    GLuint m_texTable = 0;
    GLuint m_texSignature = 0;
    GLuint m_texGreen = 0;
    GLuint m_texGray = 0;

    // gameplay variables
    float m_btnPosX = 0.0f, m_btnPosY = -0.6f, m_btnScale = 0.8f;
    float m_pattieX = 0.0f, m_pattieY = 0.0f;
    const float m_pattieSpeed = 1.2f;
    float m_cookProgress = 0.0f;
    const float m_cookRate = 0.4f;
    float m_stoveCenterX = 0.0f, m_stoveCenterY = -0.7f, m_stoveHalfW = 0.6f, m_stoveHalfH = 0.25f;
    float m_barX = 0.0f, m_barY = 0.85f, m_barScale = 0.6f;

    bool m_leftPrev = false;
};
