#pragma once
#include "VideoWrite.hpp"

struct Capture_objects
{
	std::vector<HWND> hwndCont;
	std::vector<std::shared_ptr<Hwnd2Mat>> hwnd2MatCont;
	std::vector<std::string> ObjNamesCont;
	std::vector<std::unique_ptr<VideoWrite>> WriterContainer;

	void init(size_t size, std::vector<std::wstring>& titles);
};
