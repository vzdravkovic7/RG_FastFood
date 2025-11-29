#include "../include/Renderer.h"
#include "../include/Application.h"
#include "../include/Gameplay.h"
#include "../include/Input.h"
#include <iostream>
#include <thread>


Application::Application() {}
Application::~Application() {
	if (m_window) {
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
}

bool Application::InitGLFW() {
	if (!glfwInit()) return false;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = monitor ? glfwGetVideoMode(monitor) : nullptr;
	if (mode) { m_width = mode->width; m_height = mode->height; }


	m_window = glfwCreateWindow(m_width, m_height, "RG_FastFood", nullptr, nullptr);
	if (!m_window) return false;
	glfwMakeContextCurrent(m_window);
	return true;
}

bool Application::InitGLEW() {
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "GLEW init failed: " << glewGetErrorString(err) << std::endl;
		return false;
	}
	return true;
}

int Application::Run() {
	if (!InitGLFW()) return -1;
	if (!InitGLEW()) return -1;

	Input::Init(m_window);

	// Create subsystems
	m_renderer = std::make_unique<Renderer>(m_width, m_height);
	m_gameplay = std::make_unique<Gameplay>(m_renderer.get(), m_window);


	m_lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(m_window)) {
		double frameStart = glfwGetTime();
		double now = frameStart;
		double dt = now - m_lastTime;
		m_lastTime = now;


		// ESC handling
		if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(m_window, GLFW_TRUE);
		}


		m_gameplay->Update(static_cast<float>(dt));
		m_renderer->Render(m_gameplay.get());


		glfwSwapBuffers(m_window);
		glfwPollEvents();


		double frameEnd = glfwGetTime();
		double elapsed = frameEnd - frameStart;
		if (elapsed < m_targetFrameSeconds) {
			double toSleep = m_targetFrameSeconds - elapsed;
			std::this_thread::sleep_for(std::chrono::duration<double>(toSleep));
		}
	}


	return 0;
}