#pragma once
#include <iostream>
#include <mutex>
#include <Windows.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "Hwnd2Mat.h"

class VideoWrite
{
	bool running = false;
	std::mutex mtx;
public:
	void start(const std::string& filename, HWND hwndWindow)
	{
		if (running)
		{
			//std::cout << "It's running wtf man!?\n";
			return;
		}
		mtx.lock();
		running = true;
		mtx.unlock();
		std::thread([this, filename, &hwndWindow]()
			{
				run(filename, hwndWindow);
			}).detach();
	}
	void stop()
	{
		mtx.lock();
		running = false;
		mtx.unlock();
	}
	void run(std::string filename, HWND hwndWindow)
	{
		Hwnd2Mat capDesktop(hwndWindow);
		cv::VideoWriter writer;
		int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
		double fps = 30.0;
		if (!writer.open(filename, codec, fps, capDesktop.image.size(), true))
		{
			std::cout << "WTF i cant open, HALT\n";
			return;
		}
		if (!writer.isOpened()) {
			std::cerr << "Could not open the output video file for write\n";
			return;
		}
		cv::Mat bgrImg;
		while (true)
		{
			capDesktop.read();
			cvtColor(capDesktop.image, bgrImg, cv::COLOR_BGRA2BGR);
			writer << bgrImg;
			mtx.lock();
			if (!running)
			{
				running = false;
				mtx.unlock();
				break;
			}
			mtx.unlock();
		}
	}
	~VideoWrite()
	{
		mtx.lock();
		running = false;
		mtx.unlock();
	}
};
