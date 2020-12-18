#include "WindowGameObjectHierarchy.h"
#include "imgui.h"
#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include <vector>

WindowGameObjectHierarchy::WindowGameObjectHierarchy(std::string name, int windowID) : Window(name, windowID) {
}

WindowGameObjectHierarchy::~WindowGameObjectHierarchy() {
}

void WindowGameObjectHierarchy::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	DrawGameObjectHierarchy(App->scene->GetRoot());
	ImGui::End();

	if (gameObjectDrop) gameObjectSelected->ChangeParent(gameObjectDrop);
	gameObjectDrop = nullptr;
}

void WindowGameObjectHierarchy::DrawGameObjectHierarchy(GameObject* gameObject) {
	ImGuiTreeNodeFlags	nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick ;
	if (gameObject->childs.size() == 0) nodeFlags |= ImGuiTreeNodeFlags_Leaf;

	if (gameObjectSelected == gameObject) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open = ImGui::TreeNodeEx(gameObject->GetName(), nodeFlags); //Here I'll use UUID instead of Name

	if (ImGui::IsItemClicked())	gameObjectSelected = gameObject;
	if (ImGui::BeginDragDropSource()) {
		gameObjectSelected = gameObject;
		ImGui::SetDragDropPayload("Source", &gameObject, sizeof(GameObject*));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		LOG("%s's new parent: %s", gameObjectSelected->GetName(), gameObject->GetName());
		if (ImGui::AcceptDragDropPayload("Source")) {
			if (gameObject != gameObjectSelected) {
				if (!gameObjectSelected->IsAChild(gameObject)) gameObjectDrop = gameObject;
				else LOG("You can't move a parent into a child");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (node_open) {

		for (std::vector<GameObject*>::const_iterator it = gameObject->childs.begin(); it != gameObject->childs.end(); ++it) {
			DrawGameObjectHierarchy(*it);
		}
		ImGui::TreePop();
	}
}
