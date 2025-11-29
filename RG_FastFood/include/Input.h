#pragma once
#include <GLFW/glfw3.h>


class Input {
public:
	static void InstallCursor(GLFWwindow* window, const char* cursorPath);
};