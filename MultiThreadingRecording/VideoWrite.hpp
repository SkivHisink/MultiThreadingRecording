#pragma once
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <mutex>

#include "Hwnd2Mat.hpp"

class VideoWrite
{
	bool running = false;
	std::mutex mtx;
	void run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop);
	int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
	double fps = 30.0;
public:
	// explicit
	VideoWrite() {}
	VideoWrite(VideoWrite&&)noexcept {}

	// implicit
	VideoWrite(const VideoWrite&) = delete;
	VideoWrite& operator=(const VideoWrite&) = delete;

	~VideoWrite()
	{
		mtx.lock();
		running = false;
		mtx.unlock();
	}

	bool setFPS(double fps_)
	{
		if (fps_ > 0.0 && fps_ <= 1000.0) {
			fps = fps_;
			return true;
		}
		return false;
	}
	void start(const std::string& filename, std::shared_ptr<Hwnd2Mat> capDesktop, double fps_);

	void stop();
};
