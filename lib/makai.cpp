#include "makai.hpp"
#include <stdexcept>

//using namespace Vector;

// Program

void Makai::Program::init(unsigned int width, unsigned int height, std::string windowTitle){
	// Initialize GLFW
	glfwInit();
	// Set window title to given title
	this->windowTitle = windowTitle;
	// Set GLFW version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	// Try and create window
	window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
	// If window couldn't be created, throw error
	if (window == NULL)
		throw std::runtime_error("Window could not be created.");
	// Show window
	glfwMakeContextCurrent(window);
	// Set window to input manager
	input.window = window;
	// If GLAD couldn't be initialized, throw error
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("GLAD could not be initialized.");
	// Set OpenGL viewport size to window size
	glViewport(0, 0, width, height);
	// Set STBI to flip images on load
	stbi_set_flip_vertically_on_load(true);
}

void Makai::Program::run() {
	// The delta (time between frames)
	float delta = 0.0f;
	// The last frame's time
	float last = 0.0f;
	// The physics process
	auto physFunc = [&](float delta)-> void {

	};
	// The logical process
	auto logicFunc = [&](float delta)->void {
		//Tween::yieldAllTweens();
		//Event::yieldAllTimers();
		//EntityClass::$_ROOT.yield(delta);
	};
	// While program is running...
	while(running()) {
		// Get desired framerate
		float frameDelta = (1 / frameRateMinimum);
		// Get current time
		float current = glfwGetTime();
		// Calculate delta
		delta = current - last;
		// Set last frame to current
		last = current;
		// Start threads
		std::thread physics(physFunc, delta);
		std::thread logic(logicFunc, delta);
		// Do your own stuff
		onFrame(delta);
		// Wait for threads to be done processing
		physics.join();
		logic.join();
		// Wait for frame sync, if needed
		while ((glfwGetTime() - last) < frameDelta) {
		}
		// Swap color buffer
		glfwSwapBuffers(window);
		// Check events, update window state and call callbacks
		glfwPollEvents();
	}
	// Terminate program
	terminate();
}

void Makai::Program::close() {
	// Set window to close
	glfwSetWindowShouldClose(window, true);
	// Set execution to end
	forceStop = true;
}

void Makai::Program::terminate() {
	// Call closing function
	//onClose();
	// Set window to close
	glfwSetWindowShouldClose(window, true);
	// Force execution end
	forceStop = true;
	// End GLFW
	glfwTerminate();
	// Close program
	exit(0);
}

inline bool Makai::Program::running() {
	return (glfwWindowShouldClose(window) && !forceStop);
}
/*
void Makai::Program::setClearColor(Vector4 color) {
	glClearColor(color.x, color.y, color.z, color.w);
}*/

// Input Manager

inline bool Makai::InputManager::getButtonDown(int button) {
	return glfwGetKey(window, button) == GLFW_PRESS;
}

int Makai::InputManager::getButtonState(int button) {
	// check if button was previously checked
	if (!isHeld[button]) isHeld[button] = false;
	// Get current button state (released, pressed, held, repeating)
	int state = 0;
	if (glfwGetKey(window, button) == GLFW_PRESS)
		state = (
			(glfwGetKey(window, button) == GLFW_PRESS)
		+	(isHeld[button])
		+	(glfwGetKey(window, button) == GLFW_REPEAT)
		);
	// Set current button state (held / not held)
	isHeld[button] = state;
	// Return state
	return state;
}
