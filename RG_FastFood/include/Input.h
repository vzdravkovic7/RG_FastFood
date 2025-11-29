#pragma once
#include <GLFW/glfw3.h>

class Input {
public:
    static void Init(GLFWwindow* window);

    static bool KeyDown(int key);
    static bool KeyPressed(int key);

    static void InstallCursor(GLFWwindow* window, const char* cursorPath);

private:
    static GLFWwindow* s_Window;
    static bool s_LastKeys[512];
};
