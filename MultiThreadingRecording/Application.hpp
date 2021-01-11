#pragma once
#include <vector>

#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "VideoWrite.hpp"

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

std::string wideToMultiByte(std::wstring const& wideString);

BOOL CALLBACK getOpenWindowsNames(HWND hwnd, LPARAM lParam);

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);

bool find_str_in_strCont(std::string& str, std::vector<std::string>& container);

enum recording_state
{
	record,
	pause,
	stop
};

struct time_struct
{
	std::chrono::time_point<std::chrono::steady_clock> begin;
	std::chrono::time_point<std::chrono::steady_clock> end;
	std::chrono::time_point<std::chrono::steady_clock> paused_begin;
	std::chrono::time_point<std::chrono::steady_clock> paused_end;
	std::chrono::duration<long long, std::ratio<1, 1000>> paused_summ;
	std::chrono::duration<long long, std::ratio<1, 1000>> elapsed_ms;
};

struct App_var_cont
{
	std::vector<HWND> hwndCont;
	std::vector<std::shared_ptr<Hwnd2Mat>> hwnd2MatCont;
	std::vector<std::string> ObjNamesCont;
	std::vector<std::unique_ptr<VideoWrite>> WriterContainer;
	
	void init(size_t size, std::vector<std::wstring> titles);
};

class Application final
{
	size_t number_of_threads = 0;
	// we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	std::vector<std::wstring> titles; 
	App_var_cont capture;
	double fps = 30.0;
	//UI
	GLFWwindow* window;
	ImVec4 clear_color;
	std::vector<recording_state> recording;
	std::vector<char*> save_dir_char_ptr;
	std::vector<std::string> already_captured_dir;
	std::vector<int> items;
	std::vector<std::string> already_recording;
	std::vector<time_struct> stopwatchCont;
	std::vector<bool> show_draw;

	void startButton(std::string& alert_massage, int i);
	void stopButton(std::vector<std::string>& alert_massage, int i);
	void pauseButton(std::vector<std::string>& alert_massage, int i);
	void showButton(std::vector<std::string>& alert_massage, int i);
	void captureWindow(std::vector<std::string>& alert_massage, int i);
public:
	bool init();
	void start();
	
	~Application()
	{
		for (auto ptr : save_dir_char_ptr)
		{
			delete[] ptr;
		}
	}
	
};