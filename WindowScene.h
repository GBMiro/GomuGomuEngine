#pragma once
#include "Window.h"
#include "imgui.h"
#include <string>
class WindowScene : public Window {
private:
	ImVec2 windowSize;
	ImVec2 mousePosInWindow;
	ImVec2 windowPos;
public:

	WindowScene(std::string name, int windowID);
	~WindowScene();

	void Draw() override;

	const ImVec2& GetSize()const;
	const ImVec2& GetMousePosInWindow()const;
	const ImVec2& GetPosition()const;
};

