//Opencv
#include <opencv2/imgproc.hpp>
//
#include "VideoWrite.hpp"
//ImGui
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
//additional ImGui components
#include "imgui_mod.h"
//Exception
#include "MTRException.hpp"

void VideoWrite::start(const std::string& filename, std::shared_ptr<Hwnd2Mat> capDesktop, double fps_ = 30.0)
{
	if (running)
	{
		throw MTRException("VideoWrite can't run twice");
	}
	if (!setFPS(fps_))
	{
		//This massage waiting while programmer create logger
		//std::cout << "Can't set it FPS. FPS set on 30." << std::endl;
	}
	//initialization for drawing
	glGenTextures(1, &texture);

	running = true;
	std::thread([this, filename, capDesktop]() {
		run(filename, capDesktop);
		}).detach();
}

void VideoWrite::stop()
{
	running = false;
}
void VideoWrite::pause()
{
	paused = !paused;
}

void VideoWrite::run(std::string filename, std::shared_ptr<Hwnd2Mat> capDesktop)
{
	cv::VideoWriter writer;

	if (filename.size() > 50) {
		filename = filename.substr(0, 15) + filename.substr(filename.size() - 15);
	}

	if (!writer.open(filename, codec, fps, capDesktop->getMat().size(), true)) {
		stop();
		return;
	}

	while (running) {
		if (!paused) {
			capDesktop->read();
			cvtColor(capDesktop->getMat(), img, cv::COLOR_BGRA2BGR);
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
	if (!running) {
		return;
	}
	cv::Mat image = img;
	cvtColor(image, image, cv::COLOR_BGR2RGBA);
	ImGui::Begin(window_name.c_str());
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	auto windows_size = ImGui::GetWindowSize();
	std::string chechkbox_id = "Optimize resolution " + window_name;
	ImGuiMod::Checkbox(chechkbox_id.c_str(), &optimize_res);
	if (optimize_res && windows_size.y > 60.0f) {
		cv::resize(image, image, cv::Size(windows_size.x, windows_size.y - 60.0f), 0, 0, cv::INTER_AREA);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(texture)), ImVec2(image.cols, image.rows));
	ImGui::End();
}

VideoWrite::~VideoWrite()
{
	running = false;
	Sleep(10);
}

bool VideoWrite::setFPS(double fps_)
{
	if (fps_ > 0.0 && fps_ <= 1000.0) {
		fps = fps_;
		return true;
	}
	return false;
}