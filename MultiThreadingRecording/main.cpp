#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/videoio.hpp>
#include <Windows.h>

BOOL CALLBACK speichereFenster(HWND hwnd, LPARAM lParam) {
	const DWORD TITLE_SIZE = 1024;
	WCHAR windowTitle[TITLE_SIZE];

	GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

	int length = ::GetWindowTextLength(hwnd);
	std::wstring title(&windowTitle[0]);
	if (!IsWindowVisible(hwnd) || length == 0 || title == L"Program Manager") {
		return TRUE;
	}

	// Retrieve the pointer passed into this callback, and re-'type' it.
	// The only way for a C API to pass arbitrary data is by means of a void*.
	std::vector<std::wstring>& titles = *reinterpret_cast<std::vector<std::wstring>*>(lParam);
	titles.push_back(title);

	return TRUE;
}

cv::Mat hwnd2mat(HWND hwnd)
{
	HDC hwindowDC, hwindowCompatibleDC;

	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	cv::Mat src;
	BITMAPINFOHEADER  bi;
	HBITMAP bi2;
	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = windowsize.bottom / 1;  //change this to whatever size you want to resize to
	width = windowsize.right / 1;

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 1;
	bi.biYPelsPerMeter = 2;
	bi.biClrUsed = 3;
	bi.biClrImportant = 4;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !

	src.create(height, width, CV_8UC4);
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main(int argc, char** argv)
{
	std::vector<std::wstring> titles; // we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	EnumWindows(speichereFenster, reinterpret_cast<LPARAM>(&titles));
	HWND hwndDesktop = GetDesktopWindow();
	size_t number = 0;
	int i = 0;
	for (const auto& title : titles)
		std::wcout << L"Title: " << i++ << title << std::endl;
	std::cin >> number;
	HWND hwndWindow = FindWindow(NULL, titles[number].c_str());
	cv::namedWindow("output", cv::WINDOW_NORMAL);
	cv::Mat src = hwnd2mat(/*hwndDesktop*/hwndWindow);
	cv::VideoWriter outputVideo("output.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 1, cv::Size(src.cols, src.rows));
	outputVideo.write(src);
	int key = 0;
	while (key != 27)
	{
		 src = hwnd2mat(/*hwndDesktop*/hwndWindow);
		 outputVideo.write(src);
		// you can do some image processing here
		cv::imshow("output", src);
		//cv::imshow("desktop", hwnd2mat(hwndDesktop));
		key = cv::waitKey(60); // you can change wait time
	}
	outputVideo.release();
	return 0;
}