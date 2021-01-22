#pragma once
#include "Capture_objects.h"
#include "Writing_state.h"

class GUI
{
	// we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	std::vector<std::wstring> titles;
	double fps = 30.0;
	std::vector<Writing_state> rec;
	Capture_objects capture;

	void startButton(size_t i);
	void stopButton(size_t i);
	void pauseButton(size_t i);
	void showButton(size_t i);

	void checkStart(size_t i);

	bool find_str_in_strCont(std::string& str, str_num numb);
public:
	void init(size_t number_of_threads);
	void capture_control(size_t i);
};

