#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <Windows.h>
#include <iostream>

#include "Hwnd2Mat.h"
#include "VideoWrite.hpp"

using namespace cv;

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

int main()
{
	size_t number_of_threads = std::thread::hardware_concurrency();
	if (number_of_threads == 0) {
		std::cout << "Unable to properly define or calculate the number of threads on your device. The number of threads that you can use, set to 2." << std::endl;
		number_of_threads = 2;
	}
	std::cout << "You can write no more then " << number_of_threads << std::endl;
	while (true) {
		std::cout << "Write how much windows/desktops you want to capture? ";
		int number_of_captureObject;
		std::cin >> number_of_captureObject;
		if (number_of_captureObject > 0 && number_of_captureObject <= number_of_threads)
		{
			break;
		}
		std::cout << "You write wrong number. Please try again" << std::endl;
	}
	setlocale(LC_ALL, "Russian");
	std::vector<std::wstring> titles; // we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	EnumWindows(getOpenWindowsNames, reinterpret_cast<LPARAM>(&titles));
	HWND hwndDesktop = GetDesktopWindow();
	size_t number = 0;
	int i = 0;
	for (const auto& title : titles) {
		std::wcout << i++ << " " << title << std::endl;
	}
	//std::cin >> number;
	HWND hwndWindow = FindWindow(NULL, titles[number].c_str());
	Mat ui;
	ui = Mat(40, 400, CV_8UC3, Scalar(0, 130, 0));
	putText(ui, "Press Esc to stop capturing", Point(30, 30), FONT_HERSHEY_COMPLEX, 0.7,
		Scalar(0, 0, 255), 1);

	VideoWrite vw1;
	VideoWrite vw2;

	vw1.start("desktop_capture.avi", hwndDesktop/*hwndWindow*/);
	vw2.start("desktop_capture2.avi", FindWindow(NULL, titles[number + 1].c_str()));

	while (true)
	{
		imshow("control", ui);

		int key = waitKey(5);

		if (key == '1')
		{
			std::cout << "Stop 1\n";
			vw1.stop();
		}
		if (key == '2')
		{
			std::cout << "Stop 2\n";
			vw2.stop();
		}
		if (key == 27)
			break;
	}


	return 0;
}