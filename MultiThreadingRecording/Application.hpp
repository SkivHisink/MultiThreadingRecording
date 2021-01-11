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
	recording,
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
	std::vector<recording_state> recording;
	std::vector<char*> save_dir_char_ptr;
	std::vector<std::string> already_captured_dir;
	std::vector<int> items;
	std::vector<std::string> already_recording;
	std::vector<time_struct> stopwatchCont;
	std::vector<bool> show_draw;
	void init(size_t size, std::vector<std::wstring> titles)
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
		for (size_t i = 0; i < size; ++i)
		{
			WriterContainer.push_back(std::make_unique<VideoWrite>());
			save_dir_char_ptr.push_back(new char[256]);
			save_dir_char_ptr[i][0] = '\0';
		}
		recording.assign(size, stop);
		items.assign(size, -1);
		already_recording.assign(size, "");
		already_captured_dir.assign(size, "");
		stopwatchCont.assign(size, time_struct());
		show_draw.assign(size, false);
	}
	~App_var_cont()
	{
		for (auto ptr : save_dir_char_ptr)
		{
			delete[] ptr;
		}
	}
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

public:
	bool init();
	void start();
};