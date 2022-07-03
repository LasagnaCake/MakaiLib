#include "lib/makai.hpp"
#include <stdexcept>

void Makai::Program::init(unsigned int width, unsigned int height, std::string windowTitle = "WINDOW"){
	// Initialize GLFW
	glfwInit();
	// Set window title to given title
	this->windowTitle = windowTitle;
	// Try and create window
	window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
	// If window couldn't be created, throw error
	if (window == NULL)
		throw std::runtime_error("Window could not be created.");
	// Show window
	glfwMakeContextCurrent(window);
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
		Tween::yieldAllTweens();
		Event::yieldAllTimers();
		EntityClass::$_ROOT.yield(delta);
	};
	// While program is running...
	while(running()) {
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
	}
	// Terminate program
	terminate();
}

void Makai::Program::close() {
	// Set execution to end
	forceStop = true;
}

void Makai::Program::terminate() {
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
