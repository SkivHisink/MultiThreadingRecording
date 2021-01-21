#pragma once
//std
#include <atomic>
//ImGui
#include <glad/glad.h>
//OpenCV
#include <opencv2/videoio.hpp>

#include "Hwnd2Mat.hpp"


class ImGuiDrawable
{
public:
	virtual ~ImGuiDrawable() = default;
	virtual void draw(std::string& window_name) = 0;
};

class VideoWrite final : public ImGuiDrawable
{
	std::atomic<bool> running = false;
	std::atomic<bool> paused = false;
	void run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop);
	int codec = cv::VideoWriter::fourcc('D', 'I', 'V', 'X');
	double fps = 30.0;
	cv::Mat img;
	GLuint texture;
public:
	// explicit
	VideoWrite() = default;

	// implicit
	VideoWrite(const VideoWrite&) = delete;
	VideoWrite& operator=(const VideoWrite&) {};

	~VideoWrite()
	{
		running = false;
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
	void pause();
	void draw(std::string& window_name) override;

	bool get_running() const { return running; }
};
