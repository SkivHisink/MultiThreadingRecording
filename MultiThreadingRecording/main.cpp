#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <Windows.h>
#include <iostream>

using namespace std;
using namespace cv;

BOOL CALLBACK getOpenWindowsNames(HWND hwnd, LPARAM lParam) {
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

class Hwnd2Mat final
{
public:
	Hwnd2Mat(HWND hwindow, float scale = 1);
	virtual ~Hwnd2Mat();
	virtual void read();
	Mat image;

private:
	HWND hwnd;
	HDC hwindowDC, hwindowCompatibleDC;
	int height, width, srcheight, srcwidth;
	HBITMAP hbwindow;
	BITMAPINFOHEADER  bi;
};

Hwnd2Mat::Hwnd2Mat(HWND hwindow, float scale)
{
	hwnd = hwindow;
	hwindowDC = GetDC(hwnd);
	hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	RECT windowsize;    // get the height and width of the screen
	GetClientRect(hwnd, &windowsize);

	srcheight = windowsize.bottom;
	srcwidth = windowsize.right;
	height = (int)(windowsize.bottom * scale);
	width = (int)(windowsize.right * scale);

	image.create(height, width, CV_8UC4);

	// create a bitmap
	hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);
	read();
};

void Hwnd2Mat::read()
{
	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, image.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
};

Hwnd2Mat::~Hwnd2Mat()
{
	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);
};
class VideoWrite
{
public:
	static mutex mtx;
	static void run(string filename, HWND hwndWindow)
	{
		Mat ui;
		ui = Mat(40, 400, CV_8UC3, Scalar(0, 130, 0));
		putText(ui, "Press Esc to stop capturing", Point(30, 30), FONT_HERSHEY_COMPLEX, 0.7,
			Scalar(0, 0, 255), 1);

		Mat bgrImg;

		Hwnd2Mat capDesktop(hwndWindow/*GetDesktopWindow()*/);
		VideoWriter writer;
		int codec = VideoWriter::fourcc('X', 'V', 'I', 'D');
		double fps = 30.0;
		//string filename = "./desktop_capture.avi";
		writer.open(filename, codec, fps, capDesktop.image.size(), true);
		// check if we succeeded
		if (!writer.isOpened()) {
			cerr << "Could not open the output video file for write\n";
			return /*-1*/;
		}
		while (true)
		{
			capDesktop.read();
			cvtColor(capDesktop.image, bgrImg, COLOR_BGRA2BGR);
			writer << bgrImg;
			//imshow(filename/*"desktop capture"*/, ui);
		//	int key = waitKey(5);

		//	if (key == 27)
			//	break;
		}
	}
};
int main()
{
	std::vector<std::wstring> titles; // we use std::wstring in place of std::string. This is necessary so that the entire character set can be represented.
	EnumWindows(getOpenWindowsNames, reinterpret_cast<LPARAM>(&titles));
	HWND hwndDesktop = GetDesktopWindow();
	size_t number = 0;
	int i = 0;
	for (const auto& title : titles) {
		std::wcout << i++ << " " << title << std::endl;
	}
	std::cin >> number;
	//HWND hwndWindow = FindWindow(NULL, titles[number].c_str());
	thread kek(&VideoWrite::run, "./desktop_capture.avi", hwndDesktop/*hwndWindow*/);
	thread kek2(&VideoWrite::run, "./desktop_capture2.avi", FindWindow(NULL, titles[number + 1].c_str()));
	bool start = false;
	Mat ui;
	ui = Mat(40, 400, CV_8UC3, Scalar(0, 130, 0));
	putText(ui, "Press Esc to stop capturing", Point(30, 30), FONT_HERSHEY_COMPLEX, 0.7,
		Scalar(0, 0, 255), 1);
	while (true) {
		if (!start) {
			kek.detach();
			kek2.detach();
			start = true;
		}
		imshow("filename"/*"desktop capture"*/, ui);
		int key = waitKey(5);

		if (key == 27)
			break;
	}

	//Mat bgrImg;
	//Hwnd2Mat capDesktop(hwndWindow/*GetDesktopWindow()*/);
	//Mat ui;
	//ui = Mat(40, 400, CV_8UC3, Scalar(0, 130, 0));
	//putText(ui, "Press Esc to stop capturing", Point(30, 30), FONT_HERSHEY_COMPLEX, 0.7,
	//	Scalar(0, 0, 255), 1);
	//VideoWriter writer;
	//int codec = VideoWriter::fourcc('X', 'V', 'I', 'D');
	//double fps = 30.0;
	//string filename = "./desktop_capture.avi";
	//writer.open(filename, codec, fps, capDesktop.image.size(), true);
	//// check if we succeeded
	//if (!writer.isOpened()) {
	//	cerr << "Could not open the output video file for write\n";
	//	return -1;
	//}

	//while (true)
	//{
	//	capDesktop.read();
	//	cvtColor(capDesktop.image, bgrImg, COLOR_BGRA2BGR);
	//	writer << bgrImg;
	//	imshow("desktop capture", ui);
	//	int key = waitKey(5);

	//	if (key == 27)
	//		break;
	//}
	return 0;
}