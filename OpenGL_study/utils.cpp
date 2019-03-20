#include "utils.hpp"
#include "callback.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

GLuint load_shader(const char* vert_shader_path, const char* frag_shader_path) {
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	string VertexShaderCode;
	ifstream VertexShaderStream(vert_shader_path, ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("파일 %s 를 읽을 수 없음. 정확한 디렉토리를 사용 중입니까 ? FAQ 를 우선 읽어보는 걸 잊지 마세요!\n", vert_shader_path);
		getchar();
		return 0;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(frag_shader_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// 버텍스 쉐이더를 컴파일
	printf("Compiling shader : %s\n", vert_shader_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// 버텍스 쉐이더를 검사
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// 프래그먼트 쉐이더를 컴파일
	printf("Compiling shader : %s\n", frag_shader_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// 프래그먼트 쉐이더를 검사
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// 프로그램에 링크
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// 프로그램 검사
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

bool load_obj_file(const char* obj_file_path, float *&points, float *&tex_coords,
	float *&normals, int &point_count) {
	stringstream sstr;
	ifstream obj_file_stream(obj_file_path, ios::in);
	if (obj_file_stream.is_open()) {
		sstr << obj_file_stream.rdbuf();
		obj_file_stream.close();
	}
	else {
		printf("파일 %s 를 읽을 수 없음. 정확한 디렉토리를 사용 중입니까 ? FAQ 를 우선 읽어보는 걸 잊지 마세요!\n", obj_file_path);
		getchar();
		return 0;
	}

	float *unsorted_vp_array = NULL;
	float *unsorted_vt_array = NULL;
	float *unsorted_vn_array = NULL;
	int current_unsorted_vp = 0;
	int current_unsorted_vt = 0;
	int current_unsorted_vn = 0;

	point_count = 0;
	int unsorted_vp_count = 0;
	int unsorted_vt_count = 0;
	int unsorted_vn_count = 0;
	int face_count = 0;

	string line_str;
	while (getline(sstr, line_str)) {
		if (line_str[0] == 'v') {
			if (line_str[1] == ' ') {
				unsorted_vp_count++;
			}
			else if (line_str[1] == 't') {
				unsorted_vt_count++;
			}
			else if (line_str[1] == 'n') {
				unsorted_vn_count++;
			}
		}
		else if (line_str[0] == 'f') {
			face_count++;
		}
	}

	cout << " - result - " << endl;

	cout << unsorted_vp_count << " vp" << endl;
	cout << unsorted_vt_count << " vt" << endl;
	cout << unsorted_vn_count << " vn" << endl;
	cout << face_count << " f" << endl;
	cout << "unique in obj. allocating memory..." << endl;

	unsorted_vp_array = (float*)malloc(sizeof(float) * unsorted_vp_count * 3);
	unsorted_vt_array = (float*)malloc(sizeof(float) * unsorted_vt_count * 2);
	unsorted_vn_array = (float*)malloc(sizeof(float) * unsorted_vn_count * 3);

	points = (float *)malloc(3 * face_count * 3 * sizeof(float));
	tex_coords = (float *)malloc(3 * face_count * 2 * sizeof(float));
	normals = (float *)malloc(3 * face_count * 3 * sizeof(float));

	sstr.clear();
	sstr.seekg(0);
	
	while (getline(sstr, line_str)) {
		if (line_str[0] == 'v') {
			if (line_str[1] == ' ') {
				float x, y, z;
				x = y = z = 0.0f;
				sscanf(&line_str[0], "v %f %f %f", &x, &y, &z);
				unsorted_vp_array[current_unsorted_vp * 3] = x;
				unsorted_vp_array[current_unsorted_vp * 3 + 1] = y;
				unsorted_vp_array[current_unsorted_vp * 3 + 2] = z;
				current_unsorted_vp++;
			}
			else if (line_str[1] == 't') {
				float s, t;
				s = t = 0.0f;
				sscanf(&line_str[0], "vt %f %f", &s, &t);
				unsorted_vt_array[current_unsorted_vt * 2] = s;
				unsorted_vt_array[current_unsorted_vt * 2 + 1] = t;
				current_unsorted_vt++;
			}
			else if (line_str[1] == 'n') {
				float x, y, z;
				x = y = z = 0.0f;
				sscanf(&line_str[0], "vn %f %f %f", &x, &y, &z);
				unsorted_vn_array[current_unsorted_vn * 3] = x;
				unsorted_vn_array[current_unsorted_vn * 3 + 1] = y;
				unsorted_vn_array[current_unsorted_vn * 3 + 2] = z;
				current_unsorted_vn++;
			}
		}
		else if (line_str[0] == 'f') {
			int slashCount = 0;
			int len = line_str.length();
			for (int i = 0; i < len; i++) {
				if (line_str[i] == '/') {
					slashCount++;
				}
			}
			if (slashCount != 6) {
				fprintf(stderr,
					"ERROR: file contains quads or does not match v vp/vt/vn layout - \
					make sure exported mesh is triangulated and contains vertex points, \
					texture coordinates, and normals\n");
				return false;
			}
			int vp[3], vt[3], vn[3];
			sscanf(&line_str[0], "f %i/%i/%i %i/%i/%i %i/%i/%i", &vp[0], &vt[0], &vn[0], &vp[1],
				&vt[1], &vn[1], &vp[2], &vt[2], &vn[2]);

			for (int i = 0; i < 3; i++) {
				if ((vp[i] - 1 < 0) || (vp[i] - 1 >= unsorted_vp_count)) {
					fprintf(stderr, "ERROR: invalid vertex position index in face\n");
					return false;
				}
				if ((vt[i] - 1 < 0) || (vt[i] - 1 >= unsorted_vt_count)) {
					fprintf(stderr, "ERROR: invalid texture coord index %i in face.\n",
						vt[i]);
					return false;
				}
				if ((vn[i] - 1 < 0) || (vn[i] - 1 >= unsorted_vn_count)) {
					printf("ERROR: invalid vertex normal index in face\n");
					return false;
				}
				points[point_count * 3] = unsorted_vp_array[(vp[i] - 1) * 3];
				points[point_count * 3 + 1] = unsorted_vp_array[(vp[i] - 1) * 3 + 1];
				points[point_count * 3 + 2] = unsorted_vp_array[(vp[i] - 1) * 3 + 2];
				tex_coords[point_count * 2] = unsorted_vt_array[(vt[i] - 1) * 2];
				tex_coords[point_count * 2 + 1] = unsorted_vt_array[(vt[i] - 1) * 2 + 1];
				normals[point_count * 3] = unsorted_vn_array[(vn[i] - 1) * 3];
				normals[point_count * 3 + 1] = unsorted_vn_array[(vn[i] - 1) * 3 + 1];
				normals[point_count * 3 + 2] = unsorted_vn_array[(vn[i] - 1) * 3 + 2];
				point_count++;
			}
		}
	}

	free(unsorted_vp_array);
	free(unsorted_vn_array);
	free(unsorted_vt_array);
	printf("allocated %i points\n", point_count);
	return true;
}

bool preprocess_render(GLFWwindow*& window, int width, int height) {
	glfwSetErrorCallback(show_glfw_error);
	if (!glfwInit()) {
		cerr << "GLFW 초기화 실패" << '\n';
		exit(-1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(
		800, // width
		600, // height
		"OpenGL Example",
		NULL, NULL);

	if (!window)
	{
		cerr << "윈도우 생성 실패" << '\n';
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, window_resized);
	glfwSetKeyCallback(window, key_pressed);
	glfwSwapInterval(1);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cerr << "GLEW 초기화 실패 " << glewGetErrorString(err) << '\n';
		glfwTerminate();
		exit(-1);
	}

	cout << glGetString(GL_VERSION) << '\n';

	int nr_extensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &nr_extensions);

	for (int i = 0; i < nr_extensions; ++i) {
		cout << glGetStringi(GL_EXTENSIONS, i) << '\n';
	}
}