#pragma once
#include <vector>

#include "VideoWrite.hpp"
#include <opencv2/core/mat.hpp>

struct Capture_Container
{
	std::vector<HWND> hwndCont;
	std::vector<std::shared_ptr<Hwnd2Mat>> hwnd2MatCont;
	std::vector<std::string> ObjNamesCont;
	std::vector<std::unique_ptr<VideoWrite>> WriterContainer;
	std::vector<bool> recording;
};

class Application final
{
	int number_of_threads = 0;
	int number_of_captureObject;
	std::vector<std::wstring> titles; // we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	Capture_Container capture;
	SYSTEMTIME st;
	std::string directory = "";
	double fps = 30.0;
	//UI
	cv::Mat3b ui;
	std::string winName = "Control";
	std::vector<cv::Rect> buttonContainer;
	int rows = 60;
	int cols = 450;
	int buttonWidthSize = 90;

	void onButtonCallBack(int event, int x, int y, int flags, void* userdata);
	static void s_onButtonCallBack(int event, int x, int y, int flags, void* userdata)
	{
		auto self = reinterpret_cast<Application*>(userdata);
		self->onButtonCallBack(event, x, y, flags, userdata);
	}

public:
	void init();
	void start();
};