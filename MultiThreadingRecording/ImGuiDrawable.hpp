#pragma once
#include <string>

class ImGuiDrawable
{
public:
	virtual ~ImGuiDrawable() = default;
	virtual void draw(std::string& window_name) = 0;
};
