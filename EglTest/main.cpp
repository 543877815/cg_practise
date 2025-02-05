#include <iostream>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <windows.h>

// 顶点着色器代码
const char* vertexShaderSource =
"attribute vec4 a_position;\n"
"void main() {\n"
"    gl_Position = a_position;\n"
"}\n";

// 片段着色器代码
const char* fragmentShaderSource =
"precision mediump float;\n"
"void main() {\n"
"    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

// 创建着色器
GLuint createShader(GLenum shaderType, const char* source) {
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = new char[infoLen];
			glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
			std::cerr << "Shader compilation error: " << infoLog << std::endl;
			delete[] infoLog;
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

// 创建着色器程序
GLuint createProgram() {
	GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1) {
			char* infoLog = new char[infoLen];
			glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
			std::cerr << "Program linking error: " << infoLog << std::endl;
			delete[] infoLog;
		}
		glDeleteProgram(program);
		return 0;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return program;
}

// 窗口过程函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

int main() {
	// 注册窗口类
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"EGLWindowClass";
	RegisterClass(&wc);

	// 创建窗口
	HWND hwnd = CreateWindow(wc.lpszClassName, L"EGL Triangle", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, wc.hInstance, NULL);
	if (!hwnd) {
		std::cerr << "Failed to create window" << std::endl;
		return 1;
	}

	// 显示窗口
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	// 初始化 EGL
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (display == EGL_NO_DISPLAY) {
		std::cerr << "Failed to get EGL display" << std::endl;
		return 1;
	}

	EGLint majorVersion, minorVersion;
	if (!eglInitialize(display, &majorVersion, &minorVersion)) {
		std::cerr << "Failed to initialize EGL" << std::endl;
		return 1;
	}

	std::cout << "EGL initialized successfully. Major version: " << majorVersion
		<< ", Minor version: " << minorVersion << std::endl;

	// 配置 EGL 上下文
	const EGLint configAttribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint numConfigs;
	EGLConfig config;
	if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs)) {
		std::cerr << "Failed to choose EGL config" << std::endl;
		return 1;
	}

	// 创建 EGL 上下文
	const EGLint contextAttribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	if (context == EGL_NO_CONTEXT) {
		std::cerr << "Failed to create EGL context" << std::endl;
		return 1;
	}

	// 创建 EGL 表面
	EGLSurface surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hwnd, nullptr);
	if (surface == EGL_NO_SURFACE) {
		std::cerr << "Failed to create EGL surface" << std::endl;
		return 1;
	}

	// 绑定 EGL 上下文和表面
	if (!eglMakeCurrent(display, surface, surface, context)) {
		std::cerr << "Failed to make EGL context current" << std::endl;
		return 1;
	}

	// 创建着色器程序
	GLuint program = createProgram();
	if (program == 0) {
		return 1;
	}
	glUseProgram(program);

	// 顶点数据
	GLfloat vertices[] = {
		0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	// 创建顶点缓冲区对象（VBO）
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 获取顶点属性位置
	GLint positionAttrib = glGetAttribLocation(program, "a_position");
	glEnableVertexAttribArray(positionAttrib);
	glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	// 主循环
	MSG msg;
	bool running = true;
	while (running) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				running = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// 清除颜色缓冲区
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 绘制三角形
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// 交换缓冲区
		eglSwapBuffers(display, surface);
	}

	// 释放资源
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(program);
	eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(display, surface);
	eglDestroyContext(display, context);
	eglTerminate(display);

	return 0;
}
