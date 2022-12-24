#include "window.hpp"
#include"error.hpp"
#include"camera.hpp"
#include"program.hpp"
#include <fstream>
#include <string>

//Global variables to track the movement of mouse cursor
double cursorLastX = 0.0;
double cursorLastY = 0.0;
bool bFirstCursorCallback = true; //is this the first mouse movement? (used to avoid a sudden camera jump on first frame)

void glfwErrorCallback(int aErrNum, char const* aErrDesc);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorPosCallback(GLFWwindow* window, double xPos, double yPos);
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

Window::Window(int width, int height, const char* title, bool fullscreen) : mWidth(width), mHeight(height), title(title), bFullscreen(fullscreen),
xPos(0), yPos(0), mState(nullptr), scrCounter(0)
{
	// Initialize GLFW
	if (GLFW_TRUE != glfwInit())
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwInit() failed with '%s' (%d)", msg, ecode);
	}

	glfwSetErrorCallback(&glfwErrorCallback);

	//Context related hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Framebuffer related hints
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	#if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
	#endif // ~ !NDEBUG

	//Create window (either windowed or fullscreen mode)
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (fullscreen) 
		window = glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);
	else
		window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	//Report error if window creation failed
	if (!window)
	{
		char const* msg = nullptr;
		int ecode = glfwGetError(&msg);
		throw Error("glfwCreateWindow() failed with '%s' (%d)", msg, ecode);
	}

	// Set up drawing stuff
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-Sync is on.

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress))
		throw Error("gladLoaDGLLoader() failed - cannot load GL API!");

	std::printf("RENDERER %s\n", glGetString(GL_RENDERER));
	std::printf("VENDOR %s\n", glGetString(GL_VENDOR));
	std::printf("VERSION %s\n", glGetString(GL_VERSION));
	std::printf("SHADING_LANGUAGE_VERSION %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	//Set user pointer to the current object -> a trick!
	//Allows us to associate a GLFWwindow object to the current Window object.
	//That way the Window objects will be accessible in callback functions.
	glfwSetWindowUserPointer(window, this);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	//Set callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetScrollCallback(window, scrollCallback);

	//Set viewport
	if (fullscreen)
		glViewport(0, 0, mode->width, mode->height);
	else
		glViewport(0, 0, width, height);
}

void Window::toggleFullscreen()
{
	bFullscreen = !bFullscreen;

	if(bFullscreen) //Make fullscreen
	{
		//Store current window size and position -> will be required when going back to windowed mode
		glfwGetWindowSize(window, &mWidth, &mHeight); 
		glfwGetWindowPos(window, &xPos, &yPos);

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else //Make windowed
	{
		glfwSetWindowMonitor(window, nullptr, xPos, yPos, mWidth, mHeight, 0);
	}
}


Window::~Window()
{
	printf("window destr\n");
	glfwDestroyWindow(window);
	glfwTerminate();
}

void glfwErrorCallback(int aErrNum, char const* aErrDesc)
{
	std::fprintf(stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glfwMakeContextCurrent(window);
	//Update the viewport on window resize
	glViewport(0, 0, width, height);
}

void Window::processKeyInput()
{
	if (mState)
	{
		if (mState->cam)
		{
			if (mState->cam->isActive())
			{
				float dt = mState->dt();
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
				{
					mState->cam->move(CameraMoveDir::FORWARD, dt);
				}
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
				{
					mState->cam->move(CameraMoveDir::BACKWARD, dt);
				}
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
				{
					mState->cam->move(CameraMoveDir::LEFT, dt);
				}
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
				{
					mState->cam->move(CameraMoveDir::RIGHT, dt);
				}
				if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
				{
					mState->cam->move(CameraMoveDir::UP, dt);
				}
				if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
				{
					mState->cam->move(CameraMoveDir::DOWN, dt);
				}
			}
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!win) return;

	if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
	else if (GLFW_KEY_F == key && GLFW_RELEASE == action)
	{
		win->toggleFullscreen();
		return;
	}

	State* state = win->getState();

	if (state)
	{
		// R-key reloads shaders.
		if (GLFW_KEY_R == key && GLFW_PRESS == action)
		{
			if (state->programs)
			{
				try
				{
					state->programs->reloadPrograms();
					std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
				}
				catch (std::exception const& eErr)
				{
					std::fprintf(stderr, "Error when reloading shader:\n");
					std::fprintf(stderr, "%s\n", eErr.what());
					std::fprintf(stderr, "Keeping old shader.\n");
				}
			}
		}
		else if (GLFW_KEY_ENTER == key && GLFW_PRESS == action) 
		{
			//Toggle animationw
			state->animationActive = !state->animationActive;
		}
		else if (GLFW_KEY_P == key && GLFW_PRESS == action) {
			win->takeScreenshot();
		}
		// Camera controls
		else if (state->cam) {
			//Taking the delta time since the last update, we will calculate the correct distance,
			//regardless of how many updates we did for one second (i.e. we achieve framerate independence).
			if (GLFW_KEY_SPACE == key && GLFW_PRESS == action)
			{
				bool isActive = state->cam->toggleActive();
				if(isActive)
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				else
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					bFirstCursorCallback = true;//Will prevent camera jump when the camera is activated again
				}
				return;
			}
			if (state->cam->isActive())
			{
				if (GLFW_KEY_LEFT_SHIFT == key && GLFW_RELEASE == action)
				{
					state->cam->speedUp();
				}
				else if (GLFW_KEY_LEFT_CONTROL == key && GLFW_RELEASE == action)
				{
					state->cam->speedDown();
				}
			}
		}
	}
	(void)scancode;//unused
	(void)mods;//unused
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!win) return;
	State* state = win->getState();

	if (state->cam)
	{
		if (state->cam->isActive())
		{
			//When the mouse enters the window for the first time, the xpos and ypos will be set to the cursor location. 
			//These values will be very far off from the default values of cursorLastX and cursorLastY, which will cause
			//very strong deltas. To avoid a sudden jump on the first frame, we keep track of the first mouse move and treat it
			//separately.
			//Idea taken from: https://learnopengl.com/Getting-started/Camera
			if (bFirstCursorCallback)
			{
				cursorLastX = xPos;
				cursorLastY = yPos;
				bFirstCursorCallback = false;
			}
			else
			{
				float xDelta = float(cursorLastX - xPos);
				float yDelta = float(yPos - cursorLastY); //invert y

				cursorLastX = xPos;
				cursorLastY = yPos;

				if (state)
				{
					if (state->cam)
					{
						state->cam->rotate(xDelta, yDelta);
					}
				}
			}
		}
	}

}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!win) return;
	State* state = win->getState();

	if (state)
	{
		if (state->cam)
		{
			if (state->cam->isActive())
			{
				if (yOffset < 0)
					state->cam->zoomOut();
				else if (yOffset > 0)
					state->cam->zoomIn();
			}
		}
	}
	(void)xOffset;//unused
}

void drawPPM(const char* filename, int width, int height, unsigned char* data) {
	//Set up output stream
	std::ofstream ppmFileStream(filename, std::ios::out | std::ios::binary);
	ppmFileStream << "P3\n";
	ppmFileStream << width << " " << height << "\n";
	ppmFileStream << 255 << "\n";

	//Draw
	for (int rowIdx = height-1; rowIdx >= 0; --rowIdx) {
		unsigned char* rowStart = data + rowIdx * width * 3;
		for (int colIdx = 0; colIdx < width; ++colIdx) {

			ppmFileStream << (int)rowStart[0] << " " << (int)rowStart[1] << " " << (int)rowStart[2] << "\t";
			rowStart += 3;
		}
		ppmFileStream << "\n";
	}

	ppmFileStream.close();
}

inline void Window::takeScreenshot() {
	int width{};
	int height{};
	getFramebufferSize(width, height);
	unsigned char* pixels = new unsigned char[3 * width * height];

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	drawPPM((std::string("scr_") + std::to_string(scrCounter++) + ".ppm").c_str(), width, height, pixels);

	delete[] pixels;
}

