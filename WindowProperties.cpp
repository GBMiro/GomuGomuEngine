#include "WindowProperties.h"
#include "WindowGameObjectHierarchy.h"
#include "GameObject.h"
#include "ModuleEditor.h"
#include "Mesh.h"
#include "imgui.h"
#include "Application.h"
#include "GL/glew.h"
#include "Leaks.h"
#include "ComponentTransform.h"
#include <vector>

WindowProperties::WindowProperties(std::string name, int windowID) : Window(name, windowID) {

}

WindowProperties::~WindowProperties() {
	meshes.clear();
	textures.clear();
}

void WindowProperties::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	GameObject* gameObjectSelected = App->editor->hierarchy->GetGameObjectSelected();
	if (gameObjectSelected != nullptr) {

		ImGui::BeginChild("Components", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		ImGui::PushID(gameObjectSelected);
		bool dummyActive = gameObjectSelected->active;
		if (ImGui::Checkbox("", &dummyActive)) {
			gameObjectSelected->SetActive(dummyActive);
		}
		ImGui::PopID();
		ImGui::SameLine();
		char* buff = (char*)gameObjectSelected->name.c_str();

		if (ImGui::InputText("", buff, 255)) {
			gameObjectSelected->name = buff;
		}

		for (std::vector<Component*>::const_iterator it = gameObjectSelected->components.begin(); it != gameObjectSelected->components.end(); ++it) {
			ImGui::PushID(*it);
			(*it)->DrawOnEditor();
			ImGui::PopID();
		}
		ImGui::EndChild();
	}

	ImGui::End();
}

void WindowProperties::cleanProperties() {
	textures.clear();
	meshes.clear();
}
