#pragma once
#include "GUI.hpp"
#include "Window.hpp"

class Application final
{
	size_t number_of_threads = 0;
	//UI
	Window window_control;
	GUI gui;

	bool mainCWindow(bool& show_another_window);
	void secondCWindow();
public:
	bool init();
	void start();

};