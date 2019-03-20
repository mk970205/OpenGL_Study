#pragma once
#include "include/GL/glew.h"
#include "include/GLFW//glfw3.h"
GLuint load_shader(const char* vert_shader_path, const char* frag_shader_path);

bool load_obj_file(const char* obj_file_path, float *&points, float *&tex_coords,
	float *&normals, int &point_count);

bool preprocess_render(GLFWwindow*& window, int width, int height);