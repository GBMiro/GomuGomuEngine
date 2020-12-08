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
	std::vector<GameObject*> sceneGameObjects;
	App->scene->GetSceneGameObjects(sceneGameObjects);

	for (std::vector<GameObject*>::const_iterator it = sceneGameObjects.begin(); it != sceneGameObjects.end(); ++it) {
			DrawGameObjectHierarchy(*it);
	}
	ImGui::End();
}

void WindowGameObjectHierarchy::DrawGameObjectHierarchy(GameObject* gameObject) {
	ImGuiTreeNodeFlags	node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick ;
	if (gameObject->childs.size() == 0) node_flags |= ImGuiTreeNodeFlags_Leaf;

	if (gameObjectSelected == gameObject) {
		node_flags |= ImGuiTreeNodeFlags_Selected;
	}

	bool node_open = ImGui::TreeNodeEx(gameObject->GetName(), node_flags);

	if (ImGui::IsItemClicked())	gameObjectSelected = gameObject;

	if (node_open) {

		for (std::vector<GameObject*>::const_iterator it = gameObject->childs.begin(); it != gameObject->childs.end(); ++it) {
			DrawGameObjectHierarchy(*it);
		}
		ImGui::TreePop();
	}
}
