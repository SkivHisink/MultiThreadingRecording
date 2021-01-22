#include "Application.hpp"
//ImGui components
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
//OpenCV
#include <opencv2/highgui.hpp>
//hwnd to mat transformator
#include "Hwnd2Mat.hpp"

bool Application::init()
{
	window_control.create();
	//Getting info about threads
	number_of_threads = std::thread::hardware_concurrency();
	if (number_of_threads == 0) { number_of_threads = 2; }

	//Getting info about frequency of monitor
	HDC hDCScreen = GetDC(NULL);
	monitor_freq = GetDeviceCaps(hDCScreen, VREFRESH);
	if (monitor_freq == 0)
	{
		monitor_freq = 60;
	}
	ReleaseDC(NULL, hDCScreen);

	// Initialization all objects for capturing
	gui.init(number_of_threads);

	return true;
}

void Application::start()
{
	bool show_another_window = false;
	// Main loop
	while (!window_control.windowShouldClose()) {
		glfwPollEvents();
		Sleep(1000 / monitor_freq);

		// Start the Dear ImGui frame
		window_control.start_frame();
		//Main window
		if (mainCWindow(show_another_window))
		{
			break;
		}
		//Second window
		if (show_another_window)
		{
			secondCWindow();
		}

		// Rendering
		window_control.rendering();
	}
}

bool Application::mainCWindow(bool& show_another_window)
{
	bool exit_flag = false;
	ImGui::Begin("Main Menu");
	if (ImGui::Button("Start")) {
		show_another_window = true;
	}
	if (ImGui::Button("Exit")) {
		exit_flag = true;
	}
	ImGui::End();
	return exit_flag;
}

void Application::secondCWindow()
{
	ImGui::Begin("Capture Control");
	static int threads_in_use = 1;
	if (number_of_threads == 2) {
		ImGui::Text("Unable to properly define or calculate the number of threads on your device. The number of threads that you can use, set to 2.");
	}
	else {
		ImGui::Text("Choose how much windows you want to capture. You can capture no more than %zu.", number_of_threads);
		ImGui::InputInt("Number of threads", &threads_in_use, 1, 0);
		if (threads_in_use > number_of_threads || threads_in_use <= 0) {
			threads_in_use = 1;
		}
	}
	ImGui::Text("Choose %d progs, which you want to capture", threads_in_use);
	// Child
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
		ImGui::BeginChild("ChildR", ImVec2(0, 400), true, ImGuiWindowFlags_None);
		if (ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings)) {
			for (size_t i = 0; i < threads_in_use; ++i) {
				gui.capture_control(i);
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	ImGui::End();
}