#include "../include/Util.h"
#include "../include/Input.h"
#include <iostream>
#include <cstring>

GLFWwindow* Input::s_Window = nullptr;
bool Input::s_LastKeys[512] = { false };

void Input::Init(GLFWwindow* window)
{
    s_Window = window;
    std::memset(s_LastKeys, 0, sizeof(s_LastKeys));
}

bool Input::KeyDown(int key)
{
    if (!s_Window) return false;
    int state = glfwGetKey(s_Window, key);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::KeyPressed(int key)
{
    if (!s_Window) return false;

    int state = glfwGetKey(s_Window, key);
    bool downNow = (state == GLFW_PRESS);

    bool pressed = (downNow && !s_LastKeys[key]);
    s_LastKeys[key] = downNow;

    return pressed;
}

void Input::InstallCursor(GLFWwindow* window, const char* cursorPath)
{
    if (!window) return;
    GLFWcursor* c = loadImageToCursor(cursorPath);
    if (c) {
        glfwSetCursor(window, c);
        std::cout << "Custom cursor set from: " << cursorPath << std::endl;
    }
    else {
        std::cout << "Failed to create cursor for: " << cursorPath << std::endl;
    }
}
