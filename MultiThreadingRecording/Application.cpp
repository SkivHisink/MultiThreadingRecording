#define _CRT_SECURE_NO_WARNINGS
#include "Application.hpp"

#include <iostream>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define ESC 27

namespace {
	BOOL CALLBACK getOpenWindowsNames(HWND hwnd, LPARAM lParam) {
		const DWORD TITLE_SIZE = 1024;
		WCHAR windowTitle[TITLE_SIZE];

		GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

		int length = ::GetWindowTextLength(hwnd);
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
}

void Application::onButtonCallBack(int event, int x, int y, int flags, void* userdata)
{
	int capture_ = 0;
	for (auto button : buttonContainer) {

		if (event == cv::EVENT_LBUTTONDOWN)
		{
			if (button.contains(cv::Point(x, y)))
			{
				int num = (button.y - 60) / buttonWidthSize;
				auto rect = cv::Rect(0, button.y, cols, buttonWidthSize);
				ui(rect) = cv::Vec3b(200, 200, 200);
				if (capture.recording[num]) {
					putText(ui(button), "Stopped " + capture.ObjNamesCont[num], cv::Point(30, 30), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 0, 255));
					capture.WriterContainer[num]->stop();
					capture.recording[num] = false;
					std::cout << "Capture is stopped " << capture.ObjNamesCont[num] << std::endl;
				}
				else
				{
					putText(ui(button), "Recording " + capture.ObjNamesCont[num], cv::Point(30, 30), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 0, 0));
					capture.WriterContainer[num]->start(std::to_string(st.wHour) + "_" + std::to_string(st.wMinute) + " " +
						std::to_string(st.wDay) + " " + std::to_string(st.wMonth) + " " + std::to_string(st.wYear) + " " + capture.ObjNamesCont[num] + std::to_string(num) + ".avi", capture.hwndCont[num]);
					capture.recording[num] = true;
					std::cout << "Capture is started " << capture.ObjNamesCont[num] << std::endl;
				}
			}
		}
		if (event == cv::EVENT_LBUTTONUP)
		{
		}
	}
	cv::waitKey(1);
}

void Application::init()
{
	size_t number_of_threads = std::thread::hardware_concurrency();
	if (number_of_threads == 0) {
		std::cout << "Unable to properly define or calculate the number of threads on your device. The number of threads that you can use, set to 2." << std::endl;
		number_of_threads = 2;
	}
	std::cout << "You can write no more then " << number_of_threads << std::endl;
	std::cout << "Write how much windows/desktops you want to capture? ";
	std::cin >> number_of_captureObject;
	while (std::cin.fail() || !(number_of_captureObject > 0 && number_of_captureObject <= number_of_threads))
	{
		std::cout << "You write something wrong. Please try again" << std::endl;
		std::cout << "Write how much windows/desktops you want to capture? ";
		std::cin.clear();
		std::cin.ignore(256, '\n');
		std::cin >> number_of_captureObject;
	}
}

void Application::start()
{
	EnumWindows(getOpenWindowsNames, reinterpret_cast<LPARAM>(&titles));
	GetSystemTime(&st);
	std::vector<size_t> capturedNumbers;
	int i = 1;
	std::cout << "0 Primary monitor" << std::endl;
	for (const auto& title : titles) {
		std::wcout << i++ << " " << title << std::endl;
	}
	for (int j = 0; j < number_of_captureObject; ++j) {
		std::cin >> i;
		while (std::cin.fail() || !(i >= 0 && i < titles.size())) {
			std::cout << "You write wrong number. Please try again." << std::endl;
			std::cin.clear();
			std::cin.ignore(256, '\n');
			std::cin >> i;
		}
		if (i == 0) {
			capture.hwndCont.push_back(GetDesktopWindow());
			capture.ObjNamesCont.push_back("Primary monitor");
			std::cout << "Primary monitor ready to capture" << std::endl;
		}
		else {
			capture.hwndCont.push_back(FindWindow(NULL, titles[i - 1].c_str()));
			capture.ObjNamesCont.push_back(wideToMultiByte(titles[i - 1]));
			std::cout << "\"" + capture.ObjNamesCont[j] + "\"" + " ready to capture" << std::endl;
		}
	}

	for (int k = 0; k < capture.hwndCont.size(); ++k)
	{
		capture.WriterContainer.push_back(std::make_unique<VideoWrite>());
		capture.WriterContainer[k]->start(std::to_string(st.wHour) + "_" + std::to_string(st.wMinute) + " " +
			std::to_string(st.wDay) + " " + std::to_string(st.wMonth) + " " + std::to_string(st.wYear) + " " + capture.ObjNamesCont[k] + std::to_string(k) + ".avi", capture.hwndCont[k]);
		capture.recording.push_back(true);
	}

	rows = 60 + buttonWidthSize * number_of_captureObject;
	ui = cv::Mat3b(rows, cols, cv::Vec3b(100, 255, 100));
	try {
		putText(ui, "Press ESC to stop capturing", cv::Point(30, 30), cv::FONT_HERSHEY_COMPLEX, 0.7,
			cv::Scalar(0, 0, 255), 1);
	}
	catch (cv::Exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	for (int j = 0; j < number_of_captureObject; ++j) {
		auto button = cv::Rect(0, 60 + buttonWidthSize * j, cols, buttonWidthSize);
		ui(button) = cv::Vec3b(200, 200, 200);
		try {
		putText(ui(button), "Recording " + capture.ObjNamesCont[j], cv::Point(30, 30), cv::FONT_HERSHEY_COMPLEX, 0.7, cv::Scalar(0, 0, 0));
		}
		catch (cv::Exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		buttonContainer.push_back(button);
	}

	while (true)
	{
		cv::setMouseCallback(winName, s_onButtonCallBack, this);
		try {
		imshow(winName, ui);
		}
		catch (cv::Exception& e)
		{
			std::cout << e.what() << std::endl;
		}
		int key = cv::waitKey(5);
		for (auto button : buttonContainer) {
			int num = (button.y - 60) / buttonWidthSize;
			if (key == num)
			{
				std::cout << num << " capture is stopped" << std::endl;
				capture.WriterContainer[num]->stop();
			}
		}
		if (key == ESC) {
			break;
		}
	}
}