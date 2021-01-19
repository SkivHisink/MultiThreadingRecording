#pragma once
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"

class Window
{
	GLFWwindow* window;
	ImVec4 clear_color;
public:
	bool create();
	void start_frame();
	void rendering();
	bool windowShouldClose();
	~Window();
};

