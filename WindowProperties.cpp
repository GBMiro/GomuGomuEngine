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
	////All this goes to DrawOnEditor for each component
	//if (ImGui::CollapsingHeader("Geometry")) {
	//	//if (meshes.size() == 0) App->model->getMeshes(meshes);
	//	if (meshes.size() > 0) {
	//		ImGui::Text("Total meshes: %d", meshes.size());
	//		ImGui::NewLine();
	//		for (unsigned i = 0; i < meshes.size(); ++i) {
	//			ImGui::Text("Num. Vertex: %d", meshes[i]->getNumVertex());
	//			ImGui::Text("Num. Triangles: %d", meshes[i]->getNumIndices() / 3);
	//			ImGui::Text("Num. Indices: %d", meshes[i]->getNumIndices());
	//			ImGui::Text("---------------------");
	//		}
	//	}
	//}
	//if (ImGui::CollapsingHeader("Textures")) {
	//	//if (textures.size() == 0) App->model->getTextures(textures);
	//	if (textures.size() > 0) {
	//		ImGui::Text("Total textures: %d", textures.size());
	//		ImGui::NewLine();
	//		for (unsigned i = 0; i < textures.size(); ++i) {
	//			static int textureW, textureH = 0;
	//			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureW);
	//			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureH);
	//			ImGui::Text("Width: %d", textureW);
	//			ImGui::Text("Height: %d", textureH);
	//			if (textureW > 200 || textureH > 200) {
	//				if (textureH >= textureW) {
	//					textureW = textureW * (200 / (float)textureH);
	//					textureH = 200;
	//				} else {
	//					textureH = textureH * (200 / (float)textureW);
	//					textureW = 200;
	//				}
	//			}
	//			ImGui::Image((ImTextureID)textures[i], ImVec2(textureW, textureH), ImVec2(0, 1), ImVec2(1, 0));
	//		}
	//	}
	//}
	ImGui::End();
}

void WindowProperties::cleanProperties() {
	textures.clear();
	meshes.clear();
}
