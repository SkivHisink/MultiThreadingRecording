#pragma once
//std
#include <atomic>
//ImGui
#include <glad/glad.h>
//OpenCV
#include <opencv2/videoio.hpp>
//hwnd to mat
#include "Hwnd2Mat.hpp"
//for drawing button
#include "ImGuiDrawable.hpp"

class VideoWrite final : public ImGuiDrawable
{
	std::atomic<bool> running = false;
	std::atomic<bool> paused = false;
	void run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop);
	int codec = cv::VideoWriter::fourcc('D', 'I', 'V', 'X');
	double fps = 30.0;
	cv::Mat img;
	GLuint texture;
	bool optimize_res = false;
public:
	// explicit
	VideoWrite() = default;

	// implicit
	VideoWrite(const VideoWrite&) = delete;
	VideoWrite& operator=(const VideoWrite&) = delete;

	~VideoWrite();

	bool setFPS(double fps_);
	
	void start(const std::string& filename, std::shared_ptr<Hwnd2Mat> capDesktop, double fps_);

	void stop();
	void pause();
	void draw(std::string& window_name) override;

	bool get_running() const { return running; }
};
