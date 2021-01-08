#include <iostream>
#include "VideoWrite.hpp"


void VideoWrite::start(const std::string& filename, std::shared_ptr<Hwnd2Mat> capDesktop, double fps_ = 30.0)
{
	if (running)
	{
		//std::cout << "It's running wtf man!?\n";
		return;
	}
	if(!setFPS(fps_))
	{
		std::cout << "Can't set it FPS. FPS set on 30." << std::endl;
	}
	mtx.lock();
	running = true;
	mtx.unlock();
	std::thread([this, filename, &capDesktop]()
		{
			run(filename, capDesktop);
		}).detach();
}

void VideoWrite::stop()
{
	mtx.lock();
	running = false;
	mtx.unlock();
}

void VideoWrite::run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop)
{
	cv::VideoWriter writer;
	if (filename.size() > 50) {
		filename = filename.substr(0, 15) + filename.substr(filename.size() - 15);
	}
	if (!writer.open(filename, codec, fps, capDesktop->image.size(), true)) {
		std::cout << "Can't create or open file " << filename << " for write with this parameters" << std::endl;
		stop();
		return;
	}
	cv::Mat bgrImg;
	while (true) {
		capDesktop->read();
		cvtColor(capDesktop->image, bgrImg, cv::COLOR_BGRA2BGR);
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