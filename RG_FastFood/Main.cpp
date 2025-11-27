#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono>
#include <thread>

#include "Util.h"

enum GameState {
    STATE_START_MENU,
    STATE_COOKING,
    STATE_ASSEMBLING,
    STATE_FINISHED
};

int screenWidth = 800;
int screenHeight = 600;

int endProgram(const std::string& message) {
    std::cout << message << std::endl;
    glfwTerminate();
    return -1;
}

void screenToNDC(double mx, double my, float& outX, float& outY) {
    outX = static_cast<float>((mx / screenWidth) * 2.0 - 1.0);
    outY = static_cast<float>(-((my / screenHeight) * 2.0 - 1.0));
}

int main() {
    if (!glfwInit()) return endProgram("GLFW init failed.");

    // OpenGL 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Try to get primary monitor and mode (but fallback to windowed if anything is null)
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
    if (mode) {
        screenWidth = mode->width;
        screenHeight = mode->height;
    }
    else {
        std::cout << "Warning: couldn't get primary monitor/video mode — using 800x600 windowed.\n";
        screenWidth = 800;
        screenHeight = 600;
    }

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "RG_FastFood", nullptr, nullptr);
    if (!window) return endProgram("Window creation failed.");
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::cerr << "GLEW init failed: " << glewGetErrorString(glewErr) << std::endl;
        return endProgram("GLEW init failed.");
    }
    std::cout << "GLEW initialized OK.\n";

    // Alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned int rectShader = createShader("rect.vert", "rect.frag");
    if (rectShader == 0) {
        std::cerr << "rectShader == 0 (failed to create). Check shader compile/link logs.\n";
        return endProgram("Shader creation failed.");
    }
    std::cout << "rectShader program id: " << rectShader << "\n";

    glUseProgram(rectShader);
    GLint locTex = glGetUniformLocation(rectShader, "uTex");
    GLint locTex1 = glGetUniformLocation(rectShader, "uTex1");
    if (locTex >= 0) glUniform1i(locTex, 0);
    if (locTex1 >= 0) glUniform1i(locTex1, 1);

    float verticesRect[] = {
        -0.2f,  0.2f,  0.0f, 1.0f, // top-left
        -0.2f, -0.2f,  0.0f, 0.0f, // bottom-left
         0.2f, -0.2f,  1.0f, 0.0f, // bottom-right
         0.2f,  0.2f,  1.0f, 1.0f  // top-right
    };

    unsigned int VAOrect = 0, VBOrect = 0;
    glGenVertexArrays(1, &VAOrect);
    glGenBuffers(1, &VBOrect);
    if (VAOrect == 0) std::cerr << "Warning: VAOrect == 0\n";
    if (VBOrect == 0) std::cerr << "Warning: VBOrect == 0\n";
    std::cout << "VAOrect=" << VAOrect << " VBOrect=" << VBOrect << "\n";

    glBindVertexArray(VAOrect);
    glBindBuffer(GL_ARRAY_BUFFER, VBOrect);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesRect), verticesRect, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    unsigned int texButton = loadImageToTexture("res/button_order.png");
    std::cout << "texButton = " << texButton << "\n";
    unsigned int texCursor = loadImageToTexture("res/cursor_spatula.png");
    std::cout << "texCursor = " << texCursor << "\n";
    unsigned int texPattie = loadImageToTexture("res/pattie_raw.png");
    std::cout << "texPattie = " << texPattie << "\n";

    // Custom cursor
    if (texCursor != 0) {
        GLFWcursor* spatulaCursor = loadImageToCursor("res/cursor_spatula.png");
        if (spatulaCursor) {
            glfwSetCursor(window, spatulaCursor);
            std::cout << "Custom spatula cursor set.\n";
        }
        else {
            std::cout << "Warning: spatula cursor not created.\n";
        }
    }
    else {
        std::cout << "No cursor texture found at res/cursor_spatula.png — using default OS cursor.\n";
    }

    GameState state = STATE_START_MENU;

    float btnPosX = 0.0f;
    float btnPosY = -0.6f;
    float btnScale = 0.8f;
    float quadHalfSizeX = 0.2f;
    float quadHalfSizeY = 0.2f;

    const double targetFrameSeconds = 1.0 / 75.0;

    glClearColor(0.2f, 0.4f, 0.65f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        double frameStart = glfwGetTime();

        // handle input: ESC -> close
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        // Mouse click handling (left button press)
        static bool leftPressedPrev = false;
        int leftState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        bool leftPressedNow = (leftState == GLFW_PRESS);

        if (leftPressedNow && !leftPressedPrev) {
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);
            float ndcX, ndcY;
            screenToNDC(mx, my, ndcX, ndcY);
            if (state == STATE_START_MENU) {
                float bx0 = btnPosX - quadHalfSizeX * btnScale;
                float bx1 = btnPosX + quadHalfSizeX * btnScale;
                float by0 = btnPosY - quadHalfSizeY * btnScale;
                float by1 = btnPosY + quadHalfSizeY * btnScale;
                std::cout << "Click at NDC: " << ndcX << ", " << ndcY << " btn bbox: [" << bx0 << "," << by0 << "] - [" << bx1 << "," << by1 << "]\n";
                if (ndcX >= bx0 && ndcX <= bx1 && ndcY >= by0 && ndcY <= by1) {
                    std::cout << "Order button clicked! -> go to COOKING\n";
                    state = STATE_COOKING;
                }
            }
        }
        leftPressedPrev = leftPressedNow;

        // Render
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(rectShader);
        glActiveTexture(GL_TEXTURE0);

        if (state == STATE_START_MENU) {
            if (texButton != 0) {
                glBindTexture(GL_TEXTURE_2D, texButton);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glUniform1f(glGetUniformLocation(rectShader, "uX"), btnPosX);
            glUniform1f(glGetUniformLocation(rectShader, "uY"), btnPosY);
            glUniform1f(glGetUniformLocation(rectShader, "uS"), 1.0f);
            glUniform1i(glGetUniformLocation(rectShader, "useTex1"), 0);
            glBindVertexArray(VAOrect);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }
        else if (state == STATE_COOKING) {
            if (texPattie != 0) glBindTexture(GL_TEXTURE_2D, texPattie);
            glUniform1f(glGetUniformLocation(rectShader, "uX"), 0.0f);
            glUniform1f(glGetUniformLocation(rectShader, "uY"), 0.0f);
            glUniform1f(glGetUniformLocation(rectShader, "uS"), 1.4f);
            glUniform1i(glGetUniformLocation(rectShader, "useTex1"), 0);
            glBindVertexArray(VAOrect);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        double frameEnd = glfwGetTime();
        double elapsed = frameEnd - frameStart;
        if (elapsed < targetFrameSeconds) {
            double toSleep = targetFrameSeconds - elapsed;
            std::this_thread::sleep_for(std::chrono::duration<double>(toSleep));
        }
    }

    // cleanup
    if (VBOrect) glDeleteBuffers(1, &VBOrect);
    if (VAOrect) glDeleteVertexArrays(1, &VAOrect);
    if (rectShader) glDeleteProgram(rectShader);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
