#define _CRT_SECURE_NO_WARNINGS
#include "Application.hpp"
//std
#include <iostream>
//ImGui components
#include <GLFW/glfw3.h>
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
//OpenCV
#include <opencv2/highgui.hpp>

#include "Hwnd2Mat.hpp"

std::string wideToMultiByte(std::wstring const& wideString)
{
	std::string ret;
	std::string buff(MB_CUR_MAX, '\0');

	for (wchar_t const& wc : wideString)
	{
		int mbCharLen = std::wctomb(&buff[0], wc);

		if (mbCharLen < 1) { break; }

		for (int i = 0; i < mbCharLen; ++i)
		{
			ret += buff[i];
		}
	}

	return ret;
}

BOOL CALLBACK getOpenWindowsNames(HWND hwnd, LPARAM lParam) {
	const DWORD TITLE_SIZE = 1024;
	WCHAR windowTitle[TITLE_SIZE];

	GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

	int length = GetWindowTextLength(hwnd);
	std::wstring title(&windowTitle[0]);
	if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
		return TRUE;
	}

	// Retrieve the pointer passed into this callback, and re-'type' it.
	// The only way for a C API to pass arbitrary data is by means of a void*.
	std::vector<std::wstring>& titles = *reinterpret_cast<std::vector<std::wstring>*>(lParam);
	titles.push_back(title);

	return TRUE;
}

bool find_str_in_strCont(std::string& str, std::vector<std::string>& container)
{
	for (auto str2 : container)
	{
		if (str == str2)
		{
			return true;
		}
	}
	return false;
}

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
{
	if (values.empty()) { return false; }
	return ImGui::Combo(label, currIndex, vector_getter,
		static_cast<void*>(&values), values.size());
}

void App_var_cont::init(size_t size, std::vector<std::wstring> titles)
{
	for (int i = 0; i < titles.size(); ++i) {
		if (i == 0) {
			hwndCont.push_back(GetDesktopWindow());
			ObjNamesCont.push_back("Primary monitor");
		}
		else {
			hwndCont.push_back(FindWindow(NULL, titles[i - 1].c_str()));
			ObjNamesCont.push_back(wideToMultiByte(titles[i - 1]));
		}
	}
	hwnd2MatCont.assign(size, nullptr);
	for (size_t i = 0; i < size; ++i) {
		WriterContainer.push_back(std::make_unique<VideoWrite>());
	}
}

bool Application::init()
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) { return false; }
	// Decide GL+GLSL versions
#ifdef __APPLE__
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	window = glfwCreateWindow(1280, 720, "MultiThreading VideoCapture", NULL, NULL);
	if (window == NULL) { return false; }
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
	bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
	bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
	bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
	bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
	bool err = false;
	glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
	bool err = false;
	glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
	bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
	if (err) {
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return false;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	try {
		io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Times.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);


	// Our state
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	//Getting info about threads
	number_of_threads = std::thread::hardware_concurrency();
	if (number_of_threads == 0) { number_of_threads = 2; }
	//Getting open windows 
	EnumWindows(getOpenWindowsNames, reinterpret_cast<LPARAM>(&titles));
	// Initialization all objects for capturing
	capture.init(number_of_threads, titles);
	//UI var init
	for (size_t i = 0; i < number_of_threads; ++i)
	{
		save_dir_char_ptr.push_back(new char[256]);
		save_dir_char_ptr[i][0] = '\0';
	}
	recording.assign(number_of_threads, stop);
	items.assign(number_of_threads, -1);
	already_recording.assign(number_of_threads, "");
	already_captured_dir.assign(number_of_threads, "");
	stopwatchCont.assign(number_of_threads, time_struct());
	show_draw.assign(number_of_threads, false);
	return true;
}

void Application::start()
{
	bool show_another_window = false;

	std::vector<std::string> alert_massage;
	alert_massage.assign(number_of_threads, "");

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//Main window
		{
			ImGui::Begin("Capture Control Menu");
			if (ImGui::Button("Start")) {
				show_another_window = true;
			}
			if (ImGui::Button("Exit")) {
				break;
			}
			ImGui::End();
		}
		//Second window
		if (show_another_window) {

			ImGui::Begin("Capture Control");

			static int foo = 1;

			if (number_of_threads == 2) {
				ImGui::Text("Unable to properly define or calculate the number of threads on your device. The number of threads that you can use, set to 2.");

			}
			else {
				ImGui::Text("Choose how much windows you want to capture. You can capture no more than %zu.", number_of_threads);


				ImGui::InputInt("Number of threads", &foo, 1, 0);
				if (foo > number_of_threads || foo <= 0) {
					foo = 1;
				}
			}
			ImGui::Text("Choose %d progs, which you want to capture", foo);

			// Child
			{
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
				ImGui::BeginChild("ChildR", ImVec2(0, 400), true, window_flags);
				if (ImGui::BeginTable("split", 1, ImGuiTableFlags_Resizable | ImGuiTableFlags_NoSavedSettings)) {
					for (int i = 0; i < foo; ++i) {
						std::string id_child = "ChildRCh";
						std::string id_combo = "Combo";
						std::string id_input = "Input";

						ImGui::TableNextColumn();
						ImGui::Text("Capture %d", i + 1);

						ImGui::BeginChild((id_child + std::to_string(i)).c_str(), ImVec2(0, 170), true, window_flags);
						{
							ImGui::Text("Write save directory and/or file name without .avi");
							ImGui::PushID((id_input + std::to_string(i)).c_str());
							ImGui::InputText("", save_dir_char_ptr[i], 255);
							ImGui::PopID();
							ImGui::Text("Choose window for Capture");
							ImGui::PushID((id_combo + std::to_string(i)).c_str());
							Combo("", &items[i], capture.ObjNamesCont);
							ImGui::PopID();
							ImGui::SameLine();
							stopwatchCont[i].elapsed_ms = (recording[i] != record) ? stopwatchCont[i].elapsed_ms :
								std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].end - stopwatchCont[i].begin) - stopwatchCont[i].paused_summ;
							ImGui::Text("%lld:%lld:%lld:%lld",
								stopwatchCont[i].elapsed_ms.count() / 1000 / 60 / 60,
								stopwatchCont[i].elapsed_ms.count() / 1000 / 60 % 60,
								stopwatchCont[i].elapsed_ms.count() / 1000 % 60,
								stopwatchCont[i].elapsed_ms.count() % 1000);

							startButton(alert_massage[i], i);
							//if (ImGui::Button("Start")) {
							//	auto tmp = std::string(save_dir_char_ptr[i]);
							//	if (items[i] == -1) {
							//		alert_massage[i] = "You didn't Choose any window for capture";
							//	}
							//	else if (strlen(save_dir_char_ptr[i]) == 0) {
							//		alert_massage[i] = "You didn't write name of save file";
							//	}
							//	else if (recording[i] == record) {
							//		alert_massage[i] = "You already recording " + already_recording[i];

							//	}
							//	else if (recording[i] == pause) {
							//		stopwatchCont[i].paused_end = std::chrono::steady_clock::now();
							//		stopwatchCont[i].paused_summ += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].paused_end - stopwatchCont[i].paused_begin);
							//		capture.WriterContainer[i]->pause();
							//		recording[i] = record;
							//		alert_massage[i] = "Continue recording " + already_recording[i];
							//	}
							//	else if (find_str_in_strCont(tmp, already_captured_dir)) {
							//		alert_massage[i] = "Can't record in the directory that already used";
							//	}
							//	else if (!find_str_in_strCont(capture.ObjNamesCont[items[i]], already_recording)) {
							//		stopwatchCont[i].begin = stopwatchCont[i].end = std::chrono::steady_clock::now();
							//		already_captured_dir[i] = std::string(save_dir_char_ptr[i]);
							//		recording[i] = record;
							//		alert_massage[i] = "Recording in progress";
							//		already_recording[i] = capture.ObjNamesCont[items[i]];
							//		//Start Capturing
							//		capture.hwnd2MatCont[i] = std::make_shared<Hwnd2Mat>(capture.hwndCont[items[i]]);
							//		capture.WriterContainer[i]->start(already_captured_dir[i] + ".avi", capture.hwnd2MatCont[i], fps);
							//	}
							//	else {
							//		alert_massage[i] = "This window is already captured!";
							//	}
							//}
							ImGui::SameLine();
							pauseButton(alert_massage, i);
							//if (ImGui::Button("Pause")) {
							//	if (recording[i] == record) {
							//		stopwatchCont[i].paused_begin = std::chrono::steady_clock::now();
							//		recording[i] = pause;
							//		alert_massage[i] = "Paused";
							//		// Pause
							//		capture.WriterContainer[i]->pause();
							//	}
							//	else if (recording[i] == pause) {
							//		stopwatchCont[i].paused_end = std::chrono::steady_clock::now();
							//		stopwatchCont[i].paused_summ += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].paused_end - stopwatchCont[i].paused_begin);
							//		recording[i] = record;
							//		alert_massage[i] = "Continue recording " + already_recording[i];
							//		// Unpause
							//		capture.WriterContainer[i]->pause();
							//	}
							//	else {
							//		alert_massage[i] = "Can't pause because you don't capture";
							//	}
							//}
							ImGui::SameLine();
							stopButton(alert_massage, i);
							//if (ImGui::Button("Stop")) {
							//	if (recording[i] == record || recording[i] == pause) {
							//		recording[i] = stop;
							//		already_recording[i] = "";
							//		already_captured_dir[i] = "";
							//		stopwatchCont[i].paused_summ = std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].paused_begin - stopwatchCont[i].paused_begin);;
							//		alert_massage[i] = "Stoped";
							//		// Stop
							//		capture.WriterContainer[i]->stop();
							//	}
							//	else {
							//		alert_massage[i] = "Can't stop because you don't capture";
							//	}
							//}
							ImGui::SameLine();
							showButton(alert_massage, i);
							/*if (ImGui::Button("Show")) {
								if (recording[i] == record || recording[i] == pause) {
									if (!show_draw[i]) {
										show_draw[i] = true;
									}
									else {
										show_draw[i] = false;
									}
								}
								else {
									alert_massage[i] = "Can't show because you don't capture";
								}
							}
							stopwatchCont[i].end = std::chrono::steady_clock::now();*/
							ImGui::Text(alert_massage[i].c_str());
						}
						if ((recording[i] == record || recording[i] == pause) && show_draw[i]) {
							// Draw
							capture.WriterContainer[i]->draw(already_captured_dir[i]);
						}
						ImGui::EndChild();
					}
					ImGui::EndTable();
				}
				ImGui::EndChild();
				ImGui::PopStyleVar();
			}

			ImGui::End();
		}

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Application::startButton(std::string& alert_massage, int i)
{
	if (ImGui::Button("Start")) {
		auto tmp = std::string(save_dir_char_ptr[i]);
		if (items[i] == -1) {
			alert_massage = "You didn't Choose any window for capture";
		}
		else if (strlen(save_dir_char_ptr[i]) == 0) {
			alert_massage = "You didn't write name of save file";
		}
		else if (recording[i] == record) {
			alert_massage = "You already recording " + already_recording[i];

		}
		else if (recording[i] == pause) {
			stopwatchCont[i].paused_end = std::chrono::steady_clock::now();
			stopwatchCont[i].paused_summ += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].paused_end - stopwatchCont[i].paused_begin);
			capture.WriterContainer[i]->pause();
			recording[i] = record;
			alert_massage = "Continue recording " + already_recording[i];
		}
		else if (find_str_in_strCont(tmp, already_captured_dir)) {
			alert_massage = "Can't record in the directory that already used";
		}
		else if (!find_str_in_strCont(capture.ObjNamesCont[items[i]], already_recording)) {
			stopwatchCont[i].begin = stopwatchCont[i].end = std::chrono::steady_clock::now();
			already_captured_dir[i] = std::string(save_dir_char_ptr[i]);
			recording[i] = record;
			alert_massage = "Recording in progress";
			already_recording[i] = capture.ObjNamesCont[items[i]];
			//Start Capturing
			capture.hwnd2MatCont[i] = std::make_shared<Hwnd2Mat>(capture.hwndCont[items[i]]);
			capture.WriterContainer[i]->start(already_captured_dir[i] + ".avi", capture.hwnd2MatCont[i], fps);
		}
		else {
			alert_massage = "This window is already captured!";
		}
	}
}

void Application::pauseButton(std::vector<std::string>& alert_massage, int i)
{
	if (ImGui::Button("Pause")) {
		if (recording[i] == record) {
			stopwatchCont[i].paused_begin = std::chrono::steady_clock::now();
			recording[i] = pause;
			alert_massage[i] = "Paused";
			// Pause
			capture.WriterContainer[i]->pause();
		}
		else if (recording[i] == pause) {
			stopwatchCont[i].paused_end = std::chrono::steady_clock::now();
			stopwatchCont[i].paused_summ += std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].paused_end - stopwatchCont[i].paused_begin);
			recording[i] = record;
			alert_massage[i] = "Continue recording " + already_recording[i];
			// Unpause
			capture.WriterContainer[i]->pause();
		}
		else {
			alert_massage[i] = "Can't pause because you don't capture";
		}
	}
}

void Application::stopButton(std::vector<std::string>& alert_massage, int i)
{
	if (ImGui::Button("Stop")) {
		if (recording[i] == record || recording[i] == pause) {
			recording[i] = stop;
			already_recording[i] = "";
			already_captured_dir[i] = "";
			stopwatchCont[i].paused_summ = std::chrono::duration_cast<std::chrono::milliseconds>(stopwatchCont[i].paused_begin - stopwatchCont[i].paused_begin);;
			alert_massage[i] = "Stoped";
			// Stop
			capture.WriterContainer[i]->stop();
		}
		else {
			alert_massage[i] = "Can't stop because you don't capture";
		}
	}
}

void Application::showButton(std::vector<std::string>& alert_massage, int i)
{
	if (ImGui::Button("Show")) {
		if (recording[i] == record || recording[i] == pause) {
			if (!show_draw[i]) {
				show_draw[i] = true;
			}
			else {
				show_draw[i] = false;
			}
		}
		else {
			alert_massage[i] = "Can't show because you don't capture";
		}
	}
	stopwatchCont[i].end = std::chrono::steady_clock::now();
}

