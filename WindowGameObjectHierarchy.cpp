#include "WindowGameObjectHierarchy.h"
#include "imgui.h"
#include "ModuleScene.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleCamera.h"
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
	ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (ImGui::BeginPopupContextWindow()) {
		if (ImGui::Selectable("Create GameObject")) {
			App->scene->CreateGameObject("GameObject", gameObjectSelected != nullptr ? gameObjectSelected : App->scene->GetRoot());
		}
		ImGui::EndPopup();
	}
	if (gameObjectSelected != nullptr) {
		if (ImGui::BeginPopupContextWindow()) {
			if (ImGui::Selectable("Destroy")) {
				App->scene->DestroyGameObject(gameObjectSelected);
				gameObjectSelected = nullptr;
			}
			ImGui::EndPopup();
		}
	}
	DrawGameObjectHierarchy(App->scene->GetRoot());
	//End Scrolling Region
	ImGui::EndChild();
	ImGui::End();

	if (gameObjectDrop) gameObjectSelected->ChangeParent(gameObjectDrop);
	gameObjectDrop = nullptr;
}

void WindowGameObjectHierarchy::DrawGameObjectHierarchy(GameObject* gameObject) {
	//TODO: Do not draw root node
	ImGuiTreeNodeFlags	nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (gameObject->children.size() == 0) {
		nodeFlags |= ImGuiTreeNodeFlags_Leaf;
	}

	if (gameObjectSelected == gameObject) {
		nodeFlags |= ImGuiTreeNodeFlags_Selected;
	}
	ImGui::PushID(gameObject);
	bool node_open = ImGui::TreeNodeEx(gameObject->GetName(), nodeFlags); //Here I'll use UUID instead of Name

	if (ImGui::IsItemClicked(ImGuiMouseButton(0)) || ImGui::IsItemClicked(ImGuiMouseButton(1)))	gameObjectSelected = gameObject;
	if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered()) {
		App->camera->FocusOnSelected();
	}
	if (ImGui::BeginDragDropSource()) {
		gameObjectSelected = gameObject;
		ImGui::SetDragDropPayload("Source", &gameObject, sizeof(GameObject*));
		ImGui::EndDragDropSource();
	}
	if (ImGui::BeginDragDropTarget()) {
		//LOG("%s's new parent: %s", gameObjectSelected->GetName(), gameObject->GetName());
		if (ImGui::AcceptDragDropPayload("Source")) {
			if (gameObject != gameObjectSelected) {
				if (!gameObjectSelected->IsAChild(gameObject)) gameObjectDrop = gameObject;
				else LOG("You can't move a parent into a child");
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (node_open) {

		for (std::vector<GameObject*>::const_iterator it = gameObject->children.begin(); it != gameObject->children.end(); ++it) {
			DrawGameObjectHierarchy(*it);
		}
		ImGui::TreePop();
	}
	ImGui::PopID();
}
