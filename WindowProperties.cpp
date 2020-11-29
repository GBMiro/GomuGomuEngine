#include "WindowProperties.h"
#include "Model.h"
#include "Mesh.h"
#include "imgui.h"
#include "Application.h"
#include "GL/glew.h"
#include "Leaks.h"
#include <vector>

WindowProperties::WindowProperties(std::string name, int windowID) : Window(name, windowID) {

}

WindowProperties::~WindowProperties() {

}

void WindowProperties::Draw() {
	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}
	if (ImGui::CollapsingHeader("Transformations")) {
		static float3 position = float3(0, 0, 0);
		static float3 rotation = float3(0, 0, 0);
		static float3 scale = float3(1, 1, 1);
		if (ImGui::InputFloat3("Position", &position[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		if (ImGui::InputFloat3("Rotation", &rotation[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		if (ImGui::InputFloat3("Scale", &scale[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
	}
	if (ImGui::CollapsingHeader("Geometry")) {
		if (meshes.size() == 0) App->model->getMeshes(meshes);
		if (meshes.size() > 0) {
			ImGui::Text("Total meshes: %d", meshes.size());
			ImGui::NewLine();
			for (unsigned i = 0; i < meshes.size(); ++i) {
				ImGui::Text("Num. Vertex: %d", meshes[i]->getNumVertex());
				ImGui::Text("Num. Triangles: %d", meshes[i]->getNumIndices() / 3);
				ImGui::Text("Num. Indices: %d", meshes[i]->getNumIndices());
				ImGui::Text("---------------------");
			}
		}
	}
	if (ImGui::CollapsingHeader("Textures")) {
		if (textures.size() == 0) App->model->getTextures(textures);
		if (textures.size() > 0) {
			ImGui::Text("Total textures: %d", textures.size());
			ImGui::NewLine();
			for (unsigned i = 0; i < textures.size(); ++i) {
				static int textureW, textureH = 0;
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &textureW);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &textureH);
				ImGui::Text("Width: %d", textureW);
				ImGui::Text("Height: %d", textureH);
				if (textureW > 200 || textureH > 200) {
					if (textureH >= textureW) {
						textureW = textureW * (200 / (float)textureH);
						textureH = 200;
					}
					else {
						textureH = textureH * (200 / (float)textureW);
						textureW = 200;
					}
				}
				ImGui::Image((ImTextureID)textures[i], ImVec2(textureW, textureH), ImVec2(0, 1), ImVec2(1, 0));
			}
		}
	}
	ImGui::End();
}

void WindowProperties::cleanProperties() {
	textures.clear();
	meshes.clear();
}
