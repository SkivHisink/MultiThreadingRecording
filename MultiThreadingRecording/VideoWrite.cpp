#include <iostream>

#include <opencv2/imgproc.hpp>

#include "VideoWrite.hpp"

#include "imgui/imgui.h"

void VideoWrite::start(const std::string& filename, std::shared_ptr<Hwnd2Mat> capDesktop, double fps_ = 30.0)
{
	if (running)
	{
		//std::cout << "It's running wtf man!?\n";
		return;
	}
	if (!setFPS(fps_))
	{
		std::cout << "Can't set it FPS. FPS set on 30." << std::endl;
	}
	//for drawing
	glGenTextures(1, &texture);

	running = true;
	std::thread([this, filename, capDesktop]()
		{
			run(filename, capDesktop);
		}).detach();
}

void VideoWrite::stop()
{
	running = false;
}
void VideoWrite::pause()
{
	if (paused)
	{
		paused = false;
	}
	else
	{
		paused = true;
	}
}

void VideoWrite::run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop)
{
	cv::VideoWriter writer;
	if (filename.size() > 50) {
		filename = filename.substr(0, 15) + filename.substr(filename.size() - 15);
	}
	if (!writer.open(filename, codec, fps, capDesktop->image.size(), true)) {
		stop();
		return;
	}
	while (running) {
		if (!paused) {
			capDesktop->read();
			cvtColor(capDesktop->image, img, cv::COLOR_BGRA2BGR);
			writer << img;
		}
		if (!running) {
			running = false;
			break;
		}
	}
}

void VideoWrite::draw(std::string& window_name)
{
	if (!running)
	{
		return;
	}
	cv::Mat image = img;
	cvtColor(image, image, cv::COLOR_BGR2RGBA);
	ImGui::Begin(window_name.c_str());

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(image.cols, image.rows));
	ImGui::End();
}
