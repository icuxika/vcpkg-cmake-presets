#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

namespace ui {
class UProgressBar {
  public:
	UProgressBar(float &value, int &seconds);
	void render(ImVec2 size);

  private:
	float &PValue;
	int &TotalSeconds;
	ImU32 ProgressColor = IM_COL32(30, 144, 255, 255);
	ImU32 BackgroundColor = ImGui::GetColorU32(ImGuiCol_FrameBg);

	std::string secondsToTimeString(int currentSeconds);
};
} // namespace ui
