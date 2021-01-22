#pragma once
#include <string>
#include <vector>

namespace ImGuiMod
{
	bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);
	bool Checkbox(const char* label, bool* v);
}
