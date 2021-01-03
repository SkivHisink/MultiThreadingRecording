#pragma once
#include <Windows.h>
#include <opencv2/core/mat.hpp>

class Hwnd2Mat final
{
public:
	Hwnd2Mat(HWND hwindow, float scale = 1);
	~Hwnd2Mat();
	void read();
	cv::Mat image;

private:
	HWND hwnd;
	HDC hwindowDC, hwindowCompatibleDC;
	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;
};
