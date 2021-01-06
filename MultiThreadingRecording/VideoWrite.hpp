#pragma once
#include <mutex>
#include <Windows.h>
#include <opencv2/videoio.hpp>
class VideoWrite
{
	bool running = false;
	std::mutex mtx;
public:
	// explicit
	VideoWrite() {}
	VideoWrite(VideoWrite&&) {}

	// implicit
	VideoWrite(const VideoWrite&) = delete;
	VideoWrite& operator=(const VideoWrite&) = delete;

	~VideoWrite()
	{
		mtx.lock();
		running = false;
		mtx.unlock();
	}
	
	void start(const std::string& filename, HWND hwndWindow);

	void stop();

	void run(std::string filename, HWND hwndWindow);
	
	
};
