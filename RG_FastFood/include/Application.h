#pragma once
#include <GLFW/glfw3.h>
#include <memory>


class Renderer;
class Gameplay;


class Application {
public:
	Application();
	~Application();
	int Run();


private:
	bool InitGLFW();
	bool InitGLEW();
	void MainLoop();


	GLFWwindow* m_window = nullptr;
	int m_width = 800;
	int m_height = 600;
	double m_lastTime = 0.0;
	const double m_targetFrameSeconds = 1.0 / 75.0;


	std::unique_ptr<Renderer> m_renderer;
	std::unique_ptr<Gameplay> m_gameplay;
};