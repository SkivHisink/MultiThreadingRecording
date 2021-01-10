#pragma once
#include <atomic>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "Hwnd2Mat.hpp"
#include "imgui/imgui.h"
class ImGuiDrawable
{
public:
	virtual ~ImGuiDrawable() = default;
	virtual void draw() = 0;
};
class VideoWrite final: public ImGuiDrawable
{
	std::atomic<bool> running = false;

	void run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop);
	int codec = cv::VideoWriter::fourcc('X', 'V', 'I', 'D');
	double fps = 30.0;
public:
	// explicit
	explicit VideoWrite() = default;
	VideoWrite(VideoWrite&&) noexcept {}

	// implicit
	VideoWrite(const VideoWrite&) = delete;
	VideoWrite& operator=(const VideoWrite&) = delete;

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
public:
	void draw() override
	{
		ImGui::Begin("window");
		ImGui::Text("AAAAAAAA");
		ImGui::End();
	}
};
