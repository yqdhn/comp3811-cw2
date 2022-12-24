#pragma once
#include<glad.h>
#include <GLFW/glfw3.h>
#include"../main/defaults.hpp"

//Forward declarations
class ShaderProgram;
class RenderSettings;
class Camera;
class Window;
class VertexArrayObject;

struct State
{
private:
	//Last recorded point in time. Used to calculate animation delta.
	Clock::time_point last;
	float deltaT;
public:
	State(RenderSettings* settings, Camera* cam) :programs(settings), cam(cam), deltaT(0.0f), animationActive(true) {
		last = Clock::now();
	}

	//Reset clock and update dt.
	void updateClock() {
		const auto now = Clock::now();
		float delta = std::chrono::duration_cast<Secondsf>(now - last).count();
		last = now;
		deltaT = delta;
	}

	//Get the time delta (in seconds) recorded at the last clock update. 
	float dt() {
		return deltaT;
	}

	RenderSettings* programs;
	Camera* cam;
	bool animationActive;
};

class Window
{
private:
	GLFWwindow* window;
	const char* title;
	State* mState;

	//Store the size and position of the window before entering fullscreen mode (so they can be properly restored).
	int mWidth;
	int mHeight;
	int xPos;
	int yPos;
	int scrCounter;

	bool bFullscreen;

	void processKeyInput();

public:
	Window(int width, int height, const char* title, bool fullscreen = false);

	void toggleFullscreen();
	void setState(State* state);

	void getFramebufferSize(int& width, int& height) const;
	float getAspectRatio() const;
	const char* getTitle() const;
	State* getState() const;

	bool isFullscreen() const;
	bool IsClosed() const;
	bool isMinimized() const;

	void takeScreenshot();

	//Performs a buffer swap and polls for input events.
	void updateWindow();
	GLFWwindow* getGLFWindow() const;

	~Window();
};

inline void Window::setState(State* state) {
	mState = state;
}

inline void Window::getFramebufferSize(int& width, int& height) const {
	glfwGetFramebufferSize(window, &width, &height);
}

inline float Window::getAspectRatio() const {
	int width = 0;
	int height = 0;
	getFramebufferSize(width, height);
	return width / static_cast<float>(height);
}

inline const char* Window::getTitle() const {
	return title;
}

inline State* Window::getState() const {
	return mState;
}

inline bool Window::isFullscreen() const {
	return bFullscreen;
}

inline bool Window::IsClosed() const {
	return glfwWindowShouldClose(window);
}

inline bool Window::isMinimized() const {
	return glfwGetWindowAttrib(window, GLFW_ICONIFIED);
}

inline void Window::updateWindow() {
	if (!isMinimized())
	{
		glfwSwapBuffers(window);
		processKeyInput(); //process movement keys
	}
	glfwPollEvents(); //process callbacks
}

inline GLFWwindow* Window::getGLFWindow() const {
	return window;
}
