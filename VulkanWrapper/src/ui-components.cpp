#include "ui-components.h"

namespace ui {
UProgressBar::UProgressBar(float &value, int &seconds) :
	PValue(value), TotalSeconds(seconds) {}

void UProgressBar::render(ImVec2 size) {
	ImVec2 pos = ImGui::GetCursorScreenPos();
	ImGui::InvisibleButton("##ProgressBar", size);

	if (ImGui::IsItemHovered()) {
		float mouseX = ImGui::GetMousePos().x;
		float progressBarX = pos.x;
		float progressBarWidth = size.x;
		float current = (mouseX - progressBarX) / progressBarWidth;
		float seconds = current * TotalSeconds;
		ImGui::SetTooltip("%s", secondsToTimeString(seconds).c_str());
	}

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
		float mouseX = ImGui::GetMousePos().x;
		float progressBarX = pos.x;
		float progressBarWidth = size.x;
		PValue = (mouseX - progressBarX) / progressBarWidth;
		PValue = std::clamp(PValue, 0.0f, 1.0f); // 将值夹在0和1之间
	}

	if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
		float mouseX = ImGui::GetMousePos().x;
		float progressBarX = pos.x;
		float progressBarWidth = size.x;
		PValue = (mouseX - progressBarX) / progressBarWidth;
		PValue = std::clamp(PValue, 0.0f, 1.0f); // 将值夹在0和1之间
	}

	ImGui::RenderFrame(pos, pos + size, BackgroundColor, false, 0.0f);
	ImGui::RenderFrame(pos, ImVec2(pos.x + size.x * PValue, pos.y + size.y),
		ProgressColor, false, 0.0f);
}

std::string UProgressBar::secondsToTimeString(int currentSeconds) {
	int hours = static_cast<int>(currentSeconds / 3600);
	int minutes = static_cast<int>((currentSeconds - hours * 3600) / 60);
	int seconds =
		static_cast<int>(currentSeconds - hours * 3600 - minutes * 60);

	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
	   << std::setfill('0') << minutes << ":" << std::setw(2)
	   << std::setfill('0') << seconds;
	return ss.str();
}
} // namespace ui