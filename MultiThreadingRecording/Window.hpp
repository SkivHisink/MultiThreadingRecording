#pragma once
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"

class Window final
{
	GLFWwindow* window;
	ImVec4 clear_color;
public:
	void create();
	void start_frame();
	void rendering();
	bool windowShouldClose();
	~Window();
};

