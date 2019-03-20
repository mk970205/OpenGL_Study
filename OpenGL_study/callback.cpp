#include "callback.hpp"
#include <iostream>

using namespace std;

void show_glfw_error(int error, const char* description) {
	cerr << "Error: " << description << '\n';
}

void window_resized(GLFWwindow* window, int width, int height) {
	cout << "Window resized, new window size: " << width << " x " << height << '\n';
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}

void key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == 'Q' && action == GLFW_PRESS) {
		glfwTerminate();
		exit(0);
	}
}