#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>

#include "imgui/imgui.h"

#include "VideoWrite.hpp"
#include "GUIException.hpp"

enum recording_state
{
	record,
	pause,
	stop
};

enum str_num
{
	dir,
	on_rec,
	alert_m
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

struct recording_escort_cont
{

	std::string already_captured_dir = "";//
	std::string already_recording = "";//
	std::string alert_massage = "";
	recording_state recording = stop;//
	char* save_dir = nullptr;//
	int items = -1;//
	time_struct stopwatchCont;//
	bool show_draw = false;

	std::string& operator[](str_num i)
	{
		switch (i) {
		case dir:
			return already_captured_dir;
		case on_rec:
			return already_recording;
		case alert_m:
			return alert_massage;
		default:
			throw GUIException("Out of range in recording_escort_cont operator[]");
		}
	}
};

struct App_var_cont
{
	std::vector<HWND> hwndCont;
	std::vector<std::shared_ptr<Hwnd2Mat>> hwnd2MatCont;
	std::vector<std::string> ObjNamesCont;
	std::vector<std::unique_ptr<VideoWrite>> WriterContainer;

	void init(size_t size, std::vector<std::wstring>& titles);
};
class GUI
{
	// we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	std::vector<std::wstring> titles;
	double fps = 30.0;
	std::vector<recording_escort_cont> rec;
	App_var_cont capture;

	void startButton(size_t i);
	void stopButton(size_t i);
	void pauseButton(size_t i);
	void showButton(size_t i);

	void checkStart(size_t i);

	bool find_str_in_strCont(std::string& str, str_num numb);


public:
	void init(size_t number_of_threads);
	void capturre_control(size_t i);
};

