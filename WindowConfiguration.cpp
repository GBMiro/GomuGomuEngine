#include "WindowConfiguration.h"
#include "imgui.h"
#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleTextures.h"

#include "MathGeoLib/Math/float3.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "Point.h"
#include "GL/glew.h"
#include "Leaks.h"


constexpr char* filterMinModes[] = { "Linear", "Nearest", "Linear mipmap linear", "Linear mipmap nearest", "Nearest mipmap linear", "Nearest mipmap nearest" };
constexpr char* filterMagModes[] = { "Linear", "Nearest" };
constexpr char* wrapModes[] = { "Clamp to borde", "Clamp", "Repeat", "Mirrored repeat" };

WindowConfiguration::WindowConfiguration(std::string name, int windowID) : Window(name, windowID) {
}

WindowConfiguration::~WindowConfiguration() {
}

void WindowConfiguration::Draw() {

	if (!active) return;
	if (!ImGui::Begin(name.c_str(), &active)) {
		ImGui::End();
		return;
	}

	static bool windowResizable = true;
	static bool fullscreen, windowBorderless = false;
	static int width, height;
	static float brightness = 1.0f;
	static float3 up, front, right, pos;
	static float zNear, zFar, aspectRatio, fov;
	static iPoint mouse;

	SDL_GetWindowSize(App->window->window, &width, &height);

	if (ImGui::CollapsingHeader("Editor", ImGuiTreeNodeFlags_DefaultOpen)) {

		bool dummyBool = App->editor->UseQuadTreeAcceleration();

		if (ImGui::Checkbox("Mouse Picking QuadTree acceleration", &dummyBool)) {
			App->editor->SetUseQuadTreeAcceleration(dummyBool);
		}

		dummyBool = App->editor->UseMultiMap();

		if (ImGui::Checkbox("Use MultiMap for quadTree Selection acceleration", &dummyBool)) {
			App->editor->SetUseMultiMap(dummyBool);
		}

		dummyBool = App->GetUseFrameCap();

		if (ImGui::Checkbox("Cap Frame Rate", &dummyBool)) {
			App->SetUseFrameCap(dummyBool);
		}

		int dummyFrameCap = App->GetFrameCap();


		if (ImGui::SliderInt("Max Frame Rate", &dummyFrameCap, 15, 240)) {
			App->SetFrameCap(dummyFrameCap);
		}


		dummyBool = App->editor->GetDrawQuadTree();

		if (ImGui::Checkbox("Draw Quad Tree", &dummyBool)) {
			App->editor->SetDrawQuadTree(dummyBool);
		}

		dummyBool = App->GetUseVSync();

		if (ImGui::Checkbox("VSync", &dummyBool)) {
			App->SetUseVSync(dummyBool);
		}

	}

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		up = App->camera->GetUpVector();
		front = App->camera->GetFrontVector();
		right = App->camera->GetRightVector();
		pos = App->camera->GetCameraPosition();
		aspectRatio = App->camera->GetAspectRatio();
		fov = App->camera->GetFOV();
		App->camera->GetPlanes(&zNear, &zFar);
		if (ImGui::InputFloat3("Up", &up[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		if (ImGui::InputFloat3("Front", &front[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		if (ImGui::InputFloat3("Right", &right[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		ImGui::NewLine();
		if (ImGui::InputFloat3("Position", &pos[0], "%.3f", ImGuiInputTextFlags_ReadOnly));
		ImGui::NewLine();

		float dummySpeed = App->camera->GetCameraSpeed();
		float dummmyAngleSpeed = App->camera->GetAngleSpeed();
		float dummyZoomSpeed = App->camera->GetZoomSpeed();

		if (ImGui::SliderFloat("Movement Speed", &dummySpeed, 1.0f, MaxMovSpeed)) {
			App->camera->SetCameraSpeed(dummySpeed);
		}
		if (ImGui::SliderFloat("Rotation Speed", &dummmyAngleSpeed, 1.0f, MaxRotSpeed)) {
			App->camera->SetAngleSpeed(dummmyAngleSpeed);
		}
		if (ImGui::SliderFloat("Zoom Speed", &dummyZoomSpeed, 1.0f, MaxZoomSpeed)) {
			App->camera->SetZoomSpeed(dummyZoomSpeed);
		}

		ImGui::NewLine();
		if (ImGui::SliderFloat("zNear", &zNear, 0.1f, MaxZNear)) App->camera->SetPlanes(zNear, zFar);
		if (ImGui::SliderFloat("zFar", &zFar, 0.1f, MaxZFar)) App->camera->SetPlanes(zNear, zFar);
		if (ImGui::SliderFloat("FOV", &fov, 1.0f, MaxFOV)) App->camera->SetFOV(fov);
		if (ImGui::InputFloat("Aspect Ratio", &aspectRatio, 0.0f, 0.0f, "%.2f", ImGuiInputTextFlags_ReadOnly));
	}
	if (ImGui::CollapsingHeader("Input")) {
		float mouseSensitivityDummy = App->input->GetMouseSensitivity();
		if (ImGui::InputFloat("Mouse Sensitivity", &mouseSensitivityDummy, 0, 0, "%.1f")) {
			App->input->SetMouseSensitivity(mouseSensitivityDummy);
		}

		mouse = App->input->GetMousePosition();
		float2 pos = float2(mouse.x, mouse.y);
		if (ImGui::InputFloat2("Mouse position", &pos[0], "%.0f", ImGuiInputTextFlags_ReadOnly));
	} // Mouse Position
	if (ImGui::CollapsingHeader("Renderer")) {
		static float gridColor[3] = { 0.0f, 0.0f, 1.0f };
		ImGui::ColorEdit3("Grid Color", gridColor);
		float currentColor[3];
		if (currentColor != gridColor) {
			App->renderer->SetGridColor(gridColor);
		}

		static float backgroundColor[3] = { 0.1f, 0.1f, 0.1f };
		App->renderer->GetGridColor(currentColor);
		App->renderer->GetBackgroundColor(currentColor);
		ImGui::ColorEdit3("Background Color", backgroundColor);
		if (currentColor != backgroundColor) {
			App->renderer->SetBackgroundColor(backgroundColor);
		}

		bool dummyToneMapping = App->renderer->GetUseToneMapping();

		if (ImGui::Checkbox("ToneMapping", &dummyToneMapping)) {
			App->renderer->SetUseToneMapping(dummyToneMapping);
		}

		bool dummyGammaCorrection = App->renderer->GetUseGammaCorrection();

		if (ImGui::Checkbox("Gamme Correction", &dummyGammaCorrection)) {
			App->renderer->SetUseGammaCorrection(dummyGammaCorrection);
		}
	}
	if (ImGui::CollapsingHeader("Scene Settings")) {

		float3 dummyAmbientColor = App->scene->GetAmbientColor();
		float dummyAmbientIntensity = App->scene->GetAmbientIntensity();

		if (ImGui::DragFloat3("Ambient light Color", dummyAmbientColor.ptr())) {
			App->scene->SetAmbientColor(dummyAmbientColor);
		}

		if (ImGui::DragFloat("Ambient light intensity ", &dummyAmbientIntensity, 0.01f, 0, 2.0f)) {
			App->scene->SetAmbientIntensity(dummyAmbientIntensity);
		}


		ImGui::Text("Culling Camera:");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0, 1.0, 0.0, 1.0), App->renderer->GetCullingCamera() == nullptr ? "None" : App->renderer->GetCullingCamera()->owner->GetName());
		bool dummyFrustumCulling = App->renderer->GetFrustumCulling();
		if (ImGui::Checkbox("Frustum Culling ", &dummyFrustumCulling)) {
			App->renderer->SetFrustumCulling(dummyFrustumCulling);
		}
	}
	if (ImGui::CollapsingHeader("Textures")) {
		static int indexMin = 0;
		static int indexMax = 0;
		static int indexWrap = 0;
		if (ImGui::BeginCombo("Min Filter", filterMinModes[indexMin])) {
			for (unsigned i = 0; i < IM_ARRAYSIZE(filterMinModes); ++i) {
				const bool is_selected = (indexMin == i);
				if (ImGui::Selectable(filterMinModes[i], is_selected)) {
					indexMin = i;
					ImGui::SetItemDefaultFocus();
					App->textures->SetMinFilter(indexMin);
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("Mag Filter", filterMagModes[indexMax])) {
			for (unsigned i = 0; i < IM_ARRAYSIZE(filterMagModes); ++i) {
				const bool is_selected = (indexMax == i);
				if (ImGui::Selectable(filterMagModes[i], is_selected)) {
					indexMax = i;
					ImGui::SetItemDefaultFocus();
					App->textures->SetMagFilter(indexMax);
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::BeginCombo("WRAP Mode", wrapModes[indexWrap])) {
			for (unsigned i = 0; i < IM_ARRAYSIZE(wrapModes); ++i) {
				const bool is_selected = (indexWrap == i);
				if (ImGui::Selectable(wrapModes[i], is_selected)) {
					indexWrap = i;
					ImGui::SetItemDefaultFocus();
					App->textures->SetWrapMode(indexWrap);
				}
			}
			ImGui::EndCombo();
		}
	}
	if (ImGui::CollapsingHeader("Window")) {
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) App->window->SetFlag(SDL_WINDOW_FULLSCREEN_DESKTOP, fullscreen); ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &windowResizable)) App->window->SetFlag(SDL_WINDOW_RESIZABLE, windowResizable); ImGui::SameLine();
		if (ImGui::Checkbox("Borderless", &windowBorderless)) App->window->SetFlag(SDL_WINDOW_BORDERLESS, windowBorderless);
		if (!fullscreen) {
			if (ImGui::SliderInt("Screen width", &width, 100, 1920)) App->window->SetWindowSize(width, height);
			if (ImGui::SliderInt("Screen height", &height, 100, 1080)) App->window->SetWindowSize(width, height);
		}
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.3f")) App->window->SetBrightness(brightness);
	}
	ImGui::End();
}
