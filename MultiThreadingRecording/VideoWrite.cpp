#include "VideoWrite.hpp"

#include <iostream>
#include "Hwnd2Mat.hpp"
#include <opencv2/imgproc.hpp>

void VideoWrite::start(const std::string& filename, HWND hwndWindow)
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

void VideoWrite::stop()
{
	mtx.lock();
	running = false;
	mtx.unlock();
}

void VideoWrite::run(std::string filename, HWND hwndWindow)
{
	Hwnd2Mat capDesktop(hwndWindow);
	cv::VideoWriter writer;
	int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
	double fps = 30.0;
	if (filename.size() > 50) {
		filename = filename.substr(0, 15) + filename.substr(filename.size() - 15);
	}
	if (!writer.open(filename, codec, fps, capDesktop.image.size(), true)) {
		std::cout << "Can't create or open file for write" << std::endl;//exception?!
		return;
	}
	if (!writer.isOpened()) {
		std::cerr << "Could not open the output video file for write" << std::endl;//exception?!
		return;
	}
	cv::Mat bgrImg;
	while (true) {
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