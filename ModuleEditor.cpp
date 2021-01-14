#include "ModuleEditor.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_opengl3.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "WindowMonitor.h"
#include "WindowConfiguration.h"
#include "WindowConsole.h"
#include "WindowProperties.h"
#include "WindowAbout.h"
#include "WindowGameObjectHierarchy.h"
#include "WindowScene.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "GameObject.h"
#include <iostream>
#include <algorithm>
#include "ComponentTransform.h"
#include "Leaks.h"

ModuleEditor::ModuleEditor() {

	gizmoOperation = ImGuizmo::TRANSLATE;
	gizmoMode = ImGuizmo::WORLD;

	windows.push_back(monitor = new WindowMonitor("Monitor", 0));
	windows.push_back(configuration = new WindowConfiguration("Configuration", 1));
	windows.push_back(console = new WindowConsole("Console", 2));
	windows.push_back(properties = new WindowProperties("Properties", 3));
	windows.push_back(about = new WindowAbout("About", 4));
	windows.push_back(hierarchy = new WindowGameObjectHierarchy("Hierarchy", 5));
	windows.push_back(game = new WindowScene("Scene", 6));
}

ModuleEditor::~ModuleEditor() {
	for (unsigned i = 0; i < windows.size(); ++i) {
		delete windows[i];
	}
	windows.clear();
}

bool ModuleEditor::Init() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->getContext());
	ImGui_ImplOpenGL3_Init();
	ImGuizmo::Enable(true);
	return true;
}



update_status ModuleEditor::PreUpdate() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	return UPDATE_CONTINUE;
}

void ModuleEditor::ManageGizmos() {
	GameObject* selectedObj = GetGameObjectSelected();
	if (selectedObj != nullptr) {
		ComponentTransform* selectedTransform = (ComponentTransform*)GetGameObjectSelected()->GetComponentOfType(ComponentType::CTTransform);
		if (selectedTransform) {
			ImGuizmo::SetID(0);

			ImVec2 displaySize = game->GetSize();

			float4x4 viewMatrix = App->camera->GetViewMatrix();

			viewMatrix.Transpose();

			float4x4 projectionMatrix = App->camera->GetProjectionMatrix().Transposed();

			float4x4 modelProjection = gizmoMode == ImGuizmo::MODE::LOCAL ? selectedTransform->localMatrix.Transposed() : selectedTransform->globalMatrix.Transposed();

			float modelPtr[16];
			memcpy(modelPtr, modelProjection.ptr(), 16 * sizeof(float));

			if (App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::KEY_REPEAT) {
				//ImGuizmo::SetDrawlist();
				ImGuizmo::SetRect(game->GetPosition().x, game->GetPosition().y, displaySize.x, displaySize.y);

				ImGuizmo::MODE modeToUse = gizmoOperation == ImGuizmo::OPERATION::SCALE ? ImGuizmo::MODE::LOCAL : gizmoMode;
				ImGuizmo::Manipulate(viewMatrix.ptr(), projectionMatrix.ptr(), gizmoOperation, modeToUse, modelPtr);

				if (ImGuizmo::IsUsing() == true) {
					float4x4 newMatrix;
					newMatrix.Set(modelPtr);
					modelProjection = newMatrix.Transposed();

					if (gizmoMode == ImGuizmo::MODE::LOCAL) {
						selectedTransform->SetLocalMatrix(modelProjection);
					} else {
						selectedTransform->SetGlobalMatrix(modelProjection);
					}

				} else {

					if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
						gizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
					}
					if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {
						gizmoOperation = ImGuizmo::OPERATION::ROTATE;
					}
					if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {
						gizmoOperation = ImGuizmo::OPERATION::SCALE;
					}

					if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN) {
						gizmoMode = ImGuizmo::MODE::WORLD;
					}
					if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
						gizmoMode = ImGuizmo::MODE::LOCAL;
					}
				}
			}
		}

	}
}

update_status ModuleEditor::Update() {
	int windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	static bool dockSpace = true;
	if (ImGui::Begin("Docking", &dockSpace, windowFlags)) {
		ImGui::DockSpace(ImGui::GetID("Docking"));
		if (showMainMenu() == UPDATE_STOP) return UPDATE_STOP;
		Draw();
	}

	if (App->input->GetMouseButtonDown(1) == KeyState::KEY_DOWN) {
		if (App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::KEY_REPEAT) {
			OnClicked(game->GetMousePosInWindow());
		}
	}




	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate() {
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

update_status ModuleEditor::showMainMenu() {

	update_status keepGoing = UPDATE_CONTINUE;

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit")) keepGoing = UPDATE_STOP;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			if (ImGui::MenuItem("Metrics", NULL, &monitor->active));
			if (ImGui::MenuItem("Log", NULL, &console->active));
			if (ImGui::MenuItem("Configuration", NULL, &configuration->active));
			if (ImGui::MenuItem("Properties", NULL, &properties->active));
			if (ImGui::MenuItem("Heriarchy", NULL, &hierarchy->active));
			if (ImGui::MenuItem("Scene", NULL, &game->active));
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About")) {
			if (ImGui::MenuItem("Visit github")) ShellExecuteA(NULL, "open", "https://github.com/GBMiro/GomuGomuEngine", NULL, NULL, SW_SHOWNORMAL);
			if (ImGui::MenuItem("About GomuGomuEngine", NULL, &about->active));
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	return keepGoing;
}

void ModuleEditor::Draw() {
	for (unsigned i = 0; i < windows.size(); ++i) {
		windows[i]->Draw();
	}
}

void ModuleEditor::registerFPS(float deltaTime) const {
	monitor->addData(deltaTime);
}

void ModuleEditor::registerLog(const char* log) const {

	console->addLog(log);
}

void ModuleEditor::cleanProperties() const {
	properties->cleanProperties();
}

void ModuleEditor::fileDropped(const char* filename) const {
	App->scene->AddObject(filename); // This will change to get file extension and then decide where to send it
}

GameObject* ModuleEditor::GetGameObjectSelected() const {
	return hierarchy->GetGameObjectSelected();
}

void ModuleEditor::SetGameObjectSelected(GameObject* gameObject) {
	hierarchy->SetGameObjectSelected(gameObject);
}


void ModuleEditor::OnClicked(ImVec2 mousePosInWindow) {
	if ((ImGuizmo::IsUsing()) == true)return;

	Frustum f = App->camera->GetFrustum();
	GameObject* selected = nullptr;

	ImVec2 windowSize = game->GetSize();

	float mouseNormX = (float)mousePosInWindow.x / windowSize.x;
	float mouseNormY = (float)mousePosInWindow.y / windowSize.y;

	//Normalizing mouse position in range of -1 / 1 // -1, -1 being at the bottom left corner
	mouseNormX = Lerp(-1, 1, mouseNormX);
	mouseNormY = Lerp(-1, 1, mouseNormY);

	//-MouseNormY because IMGUI inverts Y
	LineSegment picking = f.UnProjectLineSegment(mouseNormX, -mouseNormY);


	std::vector<GameObject*>possibleObjs;
	App->scene->CheckRayIntersectionWithGameObject(picking, possibleObjs, App->scene->GetRoot(), GetGameObjectSelected());

	if (possibleObjs.size() > 0) {

		std::sort(possibleObjs.begin(), possibleObjs.end(), [](GameObject* l, GameObject* r) {
			float3 frustumPosition = App->camera->GetFrustum().Pos();
			float lDist = (frustumPosition - l->GetAABB().CenterPoint()).Length();
			float rDist = (frustumPosition - r->GetAABB().CenterPoint()).Length();

			return lDist < rDist;
			});
		std::reverse(possibleObjs.begin(), possibleObjs.end());

		if (hierarchy->GetGameObjectSelected() == possibleObjs[0]) {
			for (std::vector<GameObject*>::iterator it = possibleObjs.begin(); it != possibleObjs.end() && selected == nullptr; ++it) {
				if (App->scene->CheckRayIntersectionWithMeshRenderer(picking, *it)) {
					selected = *it;
				}
			}
		} else {
			selected = possibleObjs[0];
		}
		SetGameObjectSelected(selected);

		//for (std::vector<GameObject*>::iterator it = possibleObjs.begin(); it != possibleObjs.end() && selected == nullptr; ++it) {
		//	if (App->scene->CheckRayIntersectionWithMeshRenderer(picking, *it)) {
		//		selected = *it;
		//		SetGameObjectSelected(selected);
		//	}
		//}



	}

	if (!selected) {
		SetGameObjectSelected(nullptr);
	}
}