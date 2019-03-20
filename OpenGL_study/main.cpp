#include "include/GL/glew.h"		
#include "include/GLFW/glfw3.h" 
#include "utils.hpp"
#include <iostream>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "lib/glew32.lib")
#pragma comment(lib, "lib/glfw3.lib")


using namespace std;
using namespace glm;

int main() {
	// initiallize
	float width = 800.0f, height = 600.0f;
	GLFWwindow* window;
	preprocess_render(window, width, height);

	glClearColor(0.2, 0.2, 0.2, 1);

	// vertex data
	float* vp = NULL;
	float* vt = NULL;
	float* vn = NULL;
	int point_count = 0;
	load_obj_file("sphere.obj", vp, vt, vn, point_count);

	GLuint vao = 0;
	GLuint vbo_pos = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	if (vp != NULL) {
		glGenBuffers(1, &vbo_pos);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * point_count * 3, vp, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}

	// shader program
	GLuint program_ID = load_shader("basic.vert", "basic.frag");
	GLuint model_matrix_ID = glGetUniformLocation(program_ID, "model_matrix");
	GLuint view_matrix_ID = glGetUniformLocation(program_ID, "view_matrix");
	GLuint projection_matrix_ID = glGetUniformLocation(program_ID, "projection_matrix");
	glUseProgram(program_ID);

	// matrix
	float quarternion[4];

	mat4 model_matrix = glm::identity<mat4>();

	float cam_speed = 5.0f;
	float cam_heading = 0.0f;
	float cam_heading_speed = 100.0f;

	vec3 cam_pos = vec3(0.0f, 0.0f, 5.0f);
	vec3 focus = vec3(0.0f, 0.0f, 0.0f);
	vec3 up = vec3(0.0f, 1.0f, 0.0f);
	mat4 view_matrix = lookAt(
		cam_pos,
		focus,
		up
	);

	mat4 projection_matrix = perspective(radians(67.0f), width / height, 0.1f, 100.0f);

	

	mat4 mvp = projection_matrix * view_matrix * model_matrix;

	
	glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &mvp[0][0]);

	// rendering loop
	while (!glfwWindowShouldClose(window)) {
		static double prev_sec = glfwGetTime();
		double curr_sec = glfwGetTime();
		double delta_time = curr_sec - prev_sec;
		prev_sec = curr_sec;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program_ID);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		bool cam_moved = false;

		if (glfwGetKey(window, GLFW_KEY_A)) {
			cam_pos[0] -= cam_speed * delta_time;
			focus[0] -= cam_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			cam_pos[0] += cam_speed * delta_time;
			focus[0] += cam_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_UP)) {
			cam_pos[1] += cam_speed * delta_time;
			focus[1] += cam_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN)) {
			cam_pos[1] -= cam_speed * delta_time;
			focus[1] -= cam_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_W)) {
			cam_pos[2] -= cam_speed * delta_time;
			focus[2] -= cam_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			cam_pos[2] += cam_speed * delta_time;
			focus[2] += cam_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			cam_yaw -= cam_yaw_speed * delta_time;
			cam_moved = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			cam_yaw += cam_yaw_speed * delta_time;
			cam_moved = true;
		}
		if (cam_moved) {
			cout << to_string(cam_pos) << '\n';
			view_matrix = lookAt(
				cam_pos,
				focus,
				up
			);
			mat4 view_matrix_R = rotate(glm::identity<mat4>(), cam_yaw, up);
			view_matrix = view_matrix_R * view_matrix;
			mvp = projection_matrix * view_matrix * model_matrix;
			glUniformMatrix4fv(mvp_ID, 1, GL_FALSE, &mvp[0][0]);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}