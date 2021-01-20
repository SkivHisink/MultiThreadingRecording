#include "GUI.hpp"

#include <iostream>

std::string wideToMultiByte(std::wstring const& wideString)
{
	std::string ret;
	std::string buff(MB_CUR_MAX, '\0');

	for (wchar_t const& wc : wideString)
	{
		int mbCharLen = std::wctomb(&buff[0], wc);

		if (mbCharLen < 1) { break; }

		for (int i = 0; i < mbCharLen; ++i)
		{
			ret += buff[i];
		}
	}

	return ret;
}

static auto vector_getter = [](void* vec, int idx, const char** out_text)
{
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values)
{
	if (values.empty()) { return false; }
	return ImGui::Combo(label, currIndex, vector_getter,
		static_cast<void*>(&values), values.size());
}

BOOL CALLBACK getOpenWindowsNames(HWND hwnd, LPARAM lParam) {
	const DWORD TITLE_SIZE = 1024;
	WCHAR windowTitle[TITLE_SIZE];

	GetWindowTextW(hwnd, windowTitle, TITLE_SIZE);

	int length = GetWindowTextLength(hwnd);
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

bool updateWindows(App_var_cont& capture, std::vector<std::wstring>& titles)
{
	if (titles.size() != capture.hwndCont.size()) {
		capture.hwndCont.clear();
		capture.ObjNamesCont.clear();
		for (size_t i = 0; i < titles.size(); ++i) {
			if (i == 0) {
				capture.hwndCont.push_back(GetDesktopWindow());
				capture.ObjNamesCont.push_back("Primary monitor");
			}
			else {
				capture.hwndCont.push_back(FindWindow(NULL, titles[i - 1].c_str()));
				capture.ObjNamesCont.push_back(wideToMultiByte(titles[i - 1]));
			}
		}
		return true;
	}
	return false;
}
void App_var_cont::init(size_t size, std::vector<std::wstring>& titles)
{
	updateWindows(*this, titles);
	hwnd2MatCont.assign(size, nullptr);
	for (size_t i = 0; i < size; ++i) {
		WriterContainer.push_back(std::make_unique<VideoWrite>());
	}
}

void GUI::capturre_control(size_t i)
{
	std::string id_child = "ChildRCh";
	std::string id_combo = "Combo";
	std::string id_input = "Input";

	ImGui::TableNextColumn();
	ImGui::Text("Capture %zu", i + 1);

	ImGui::BeginChild((id_child + std::to_string(i)).c_str(), ImVec2(0, 170), true, ImGuiWindowFlags_None);
	{
		ImGui::Text("Write save directory and/or file name without .avi");
		ImGui::PushID((id_input + std::to_string(i)).c_str());
		ImGui::InputText("", rec[i].save_dir, 255);
		ImGui::PopID();
		ImGui::Text("Choose window for Capture");
		ImGui::PushID((id_combo + std::to_string(i)).c_str());
		Combo("", &rec[i].items, capture.ObjNamesCont);
		ImGui::PopID();
		ImGui::SameLine();
		rec[i].stopwatchCont.elapsed_ms = (rec[i].recording != record) ? rec[i].stopwatchCont.elapsed_ms :
			std::chrono::duration_cast<std::chrono::milliseconds>(rec[i].stopwatchCont.end - rec[i].stopwatchCont.begin) - rec[i].stopwatchCont.paused_summ;
		ImGui::Text("%lld:%lld:%lld:%lld",
			rec[i].stopwatchCont.elapsed_ms.count() / 1000 / 60 / 60,
			rec[i].stopwatchCont.elapsed_ms.count() / 1000 / 60 % 60,
			rec[i].stopwatchCont.elapsed_ms.count() / 1000 % 60,
			rec[i].stopwatchCont.elapsed_ms.count() % 1000);

		startButton(i);
		checkStart(i);
		ImGui::SameLine();
		pauseButton(i);
		ImGui::SameLine();
		stopButton(i);
		ImGui::SameLine();
		showButton(i);
		rec[i].stopwatchCont.end = std::chrono::steady_clock::now();
		ImGui::Text(rec[i].alert_massage.c_str());
	}
	if ((rec[i].recording == record || rec[i].recording == pause) && rec[i].show_draw) {
		capture.WriterContainer[i]->draw(rec[i].already_captured_dir);
	}
	ImGui::EndChild();
}

void GUI::init(size_t number_of_threads)
{
	EnumWindows(getOpenWindowsNames, reinterpret_cast<LPARAM>(&titles));
	capture.init(number_of_threads, titles);
	//UI var init
	rec.assign(number_of_threads, recording_escort_cont());
	for (size_t i = 0; i < number_of_threads; ++i)
	{
		rec[i].save_dir = new char[256];
		if (rec[i].save_dir == nullptr)
		{
			throw GUIException("Bad memory allocation for save_dir");
		}
		rec[i].save_dir[0] = '\0';
	}
}

bool GUI::find_str_in_strCont(std::string& str, str_num numb)
{
	for (int i = 0; i < rec.size(); ++i)
	{
		if (str == rec[i][numb])
		{
			return true;
		}
	}
	return false;
}

void GUI::startButton(size_t i)
{
	if (ImGui::Button("Start")) {
		std::string tmp = std::string(rec[i].save_dir);
		if (rec[i].items == -1) {
			rec[i].alert_massage = "You didn't Choose any window for capture";
		}
		else if (strlen(rec[i].save_dir) == 0) {
			rec[i].alert_massage = "You didn't write name of save file";
		}
		else if (rec[i].recording == record) {
			rec[i].alert_massage = "You already recording " + rec[i].already_recording;
		}
		else if (rec[i].recording == pause) {
			rec[i].stopwatchCont.paused_end = std::chrono::steady_clock::now();
			rec[i].stopwatchCont.paused_summ += std::chrono::duration_cast<std::chrono::milliseconds>(rec[i].stopwatchCont.paused_end - rec[i].stopwatchCont.paused_begin);
			capture.WriterContainer[i]->pause();
			rec[i].recording = record;
			rec[i].alert_massage = "Continue recording " + rec[i].already_recording[i];
		}
		else if (find_str_in_strCont(tmp, dir)) {
			rec[i].alert_massage = "Can't record in the directory that already used";
		}
		else if (!find_str_in_strCont(capture.ObjNamesCont[rec[i].items], on_rec)) {
			rec[i].stopwatchCont.begin = rec[i].stopwatchCont.end = std::chrono::steady_clock::now();
			rec[i].already_captured_dir = std::string(rec[i].save_dir);
			rec[i].recording = record;
			rec[i].alert_massage = "Recording in progress";
			rec[i].already_recording = capture.ObjNamesCont[rec[i].items];
			//Start Capturing
			capture.hwnd2MatCont[i] = std::make_shared<Hwnd2Mat>(capture.hwndCont[rec[i].items]);
			capture.WriterContainer[i]->start(rec[i].already_captured_dir + ".avi", capture.hwnd2MatCont[i], fps);
		}
		else {
			rec[i].alert_massage = "This window is already captured!";
		}
	}
}

void GUI::checkStart(size_t i)
{
	if (!capture.WriterContainer[i]->get_running() && rec[i].recording == record) {
		rec[i].alert_massage = "Can't create or open file ";
		rec[i].recording == false;
	}
}
void GUI::pauseButton(size_t i)
{
	if (ImGui::Button("Pause")) {
		if (rec[i].recording == record) {
			rec[i].stopwatchCont.paused_begin = std::chrono::steady_clock::now();
			rec[i].recording = pause;
			rec[i].alert_massage = "Paused";
			// Pause
			capture.WriterContainer[i]->pause();
		}
		else if (rec[i].recording == pause) {
			rec[i].stopwatchCont.paused_end = std::chrono::steady_clock::now();
			rec[i].stopwatchCont.paused_summ += std::chrono::duration_cast<std::chrono::milliseconds>(rec[i].stopwatchCont.paused_end - rec[i].stopwatchCont.paused_begin);
			rec[i].recording = record;
			rec[i].alert_massage = "Continue recording " + rec[i].already_recording;
			// Unpause
			capture.WriterContainer[i]->pause();
		}
		else {
			rec[i].alert_massage = "Can't pause because you don't capture";
		}
	}
}

void GUI::stopButton(size_t i)
{
	if (ImGui::Button("Stop")) {
		if (rec[i].recording == record || rec[i].recording == pause) {
			rec[i].recording = stop;
			rec[i].already_recording = "";
			rec[i].already_captured_dir = "";
			rec[i].stopwatchCont.paused_summ = std::chrono::duration_cast<std::chrono::milliseconds>(rec[i].stopwatchCont.paused_begin - rec[i].stopwatchCont.paused_begin);;
			rec[i].alert_massage = "Stoped";
			// Stop
			capture.WriterContainer[i]->stop();
		}
		else {
			rec[i].alert_massage = "Can't stop because you don't capture";
		}
	}
}

void GUI::showButton(size_t i)
{
	if (ImGui::Button("Show")) {
		if (rec[i].recording == record || rec[i].recording == pause) {
			if (!rec[i].show_draw) {
				rec[i].show_draw = true;
			}
			else {
				rec[i].show_draw = false;
			}
		}
		else {
			rec[i].alert_massage = "Can't show because you don't capture";
		}
	}
	rec[i].stopwatchCont.end = std::chrono::steady_clock::now();
}