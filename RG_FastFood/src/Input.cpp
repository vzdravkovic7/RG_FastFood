#include "../include/Util.h"
#include "../include/Input.h"
#include <iostream>


void Input::InstallCursor(GLFWwindow* window, const char* cursorPath) {
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