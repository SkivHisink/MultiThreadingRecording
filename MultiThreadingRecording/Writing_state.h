#pragma once
#include <string>

#include "GUIException.hpp"
#include "Time_struct.h"

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

struct Writing_state
{

	std::string already_captured_dir = "";//
	std::string already_recording = "";//
	std::string alert_massage = "";
	recording_state recording = stop;//
	char* save_dir = nullptr;//
	int items = -1;//
	Time_struct stopwatchCont;//
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
			throw GUIException("Out of range in Writing_state operator[]");
		}
	}
};