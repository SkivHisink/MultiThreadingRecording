#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_mod.h"

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

bool ImGuiMod::Combo(const char* label, int* currIndex, std::vector<std::string>& values)
{
	if (values.empty()) { return false; }
	return ImGui::Combo(label, currIndex, [](void* vec, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
		}, static_cast<void*>(&values), static_cast<int>(values.size()));
}

bool ImGuiMod::Checkbox(const char* label, bool* v)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems) {
		return false;
	}
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = ImGuiStyle::ImGuiStyle();
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	const ImVec2 pading = ImVec2(2, 2);
	const ImRect check_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(label_size.y + style.FramePadding.x * 6, label_size.y + style.FramePadding.y / 2));
	ImGui::ItemSize(check_bb, style.FramePadding.y);
	ImRect total_bb = check_bb;
	if (label_size.x > 0) {
		ImGui::SameLine(0, style.ItemInnerSpacing.x);
	}
	const ImRect text_bb(window->DC.CursorPos + ImVec2(0, style.FramePadding.y), window->DC.CursorPos + ImVec2(0, style.FramePadding.y) + label_size);
	if (label_size.x > 0){
		ImGui::ItemSize(ImVec2(text_bb.GetWidth(), check_bb.GetHeight()), style.FramePadding.y);
		total_bb = ImRect(ImMin(check_bb.Min, text_bb.Min), ImMax(check_bb.Max, text_bb.Max));
	}
	if (!ImGui::ItemAdd(total_bb, id)) {
		return false;
	}
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed) {
		*v = !(*v);
	}
	const float check_sz = ImMin(check_bb.GetWidth(), check_bb.GetHeight());
	const float check_sz2 = check_sz / 2;
	const float pad = ImMax(1.0f, (float)(int)(check_sz / 4.f));
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 5, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 4, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 3, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 2, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 1, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x / 2, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 1, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 2, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 3, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 4, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 5, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), 12);
	if (*v){
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 5, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 4, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 3, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 2, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 1, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x / 2, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 1, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x / 2 - 2, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 3, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 4, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 5, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(0.71f, 0.18f, 0.29f, 1.00f)), 12);
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 + 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(1.00f, 1.00f, 1.00f, 1.00f)), 12);
	}
	else{
		window->DrawList->AddCircleFilled(ImVec2(check_bb.Min.x + (check_bb.Max.x - check_bb.Min.x) / 2 - 6, check_bb.Min.y + 9), 7, ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), 12);
	}
	if (label_size.x > 0.0f)
		ImGui::RenderText(text_bb.GetTL(), label);
	return pressed;
}