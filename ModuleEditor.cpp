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
#include "Quadtree.h"
#include "ImporterTextures.h"
#include "PreciseTimer.h"
#include "Mesh.h"
#include "ComponentMeshRenderer.h"
#include "Leaks.h"

ModuleEditor::ModuleEditor() {
	useQuadTreeAcceleration = useMultiMap = true;
	drawQuadTree = false;
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

void ModuleEditor::OnMouseMotion() {
	previouslySelectedGameObjects.clear();
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

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::KEY_DOWN) {
		App->scene->DestroyGameObject(GetGameObjectSelected());
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
			if (ImGui::MenuItem("Save scene")) App->scene->SaveScene("Assets/Library/Scenes/userScene.fbx");
			if (ImGui::MenuItem("Load default scene")) SetSceneToLoad(DEFAULT_SCENE);
			if (ImGui::MenuItem("Load last saved scene")) SetSceneToLoad(USER_SCENE);
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
		ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
		std::string tag;
		if (state == PLAY) tag = "Stop";
		else tag = "Play";
		if (ImGui::Button(tag.c_str())) {
			if (state == PLAY) {
				state = STOP;
				SetSceneToLoad(TEMPORAL_SCENE);
				LOG("Previous scene loaded");
			} else {
				state = PLAY;
				App->scene->SaveScene("Assets/Library/Scenes/temporalScene.fbx");
				LOG("Scene saved");
			}
		}
		if (ImGui::Button("Pause"));
		if (ImGui::Button("Step"));
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
	std::string file(filename);
	int posExtension = file.find_last_of(".") + 1;
	std::string extension = file.substr(posExtension);
	if (_stricmp(extension.c_str(), "fbx") == 0) {
		App->scene->AddObject(filename);
	}
	else if (_stricmp(extension.c_str(), "dds") == 0 || _stricmp(extension.c_str(), "png") == 0 || _stricmp(extension.c_str(), "jpg") == 0 || _stricmp(extension.c_str(), "tif") == 0) {
		ImporterTextures::ImportTexture(filename);
	}
}

GameObject* ModuleEditor::GetGameObjectSelected() const {
	return hierarchy->GetGameObjectSelected();
}

void ModuleEditor::SetGameObjectSelected(GameObject* gameObject) {
	hierarchy->SetGameObjectSelected(gameObject);
}

bool ModuleEditor::UseQuadTreeAcceleration()const {
	return useQuadTreeAcceleration;
}

bool ModuleEditor::UseMultiMap()const {
	return useMultiMap;
}

void ModuleEditor::SetUseMultiMap(bool should) {
	useMultiMap = should;
}


void ModuleEditor::SetUseQuadTreeAcceleration(bool should) {
	useQuadTreeAcceleration = should;
}

GameObject* ModuleEditor::TryToSelectGameObject(const LineSegment& picking, bool useQuadTreeAccel) {
	GameObject* selected = nullptr;

	if (!useQuadTreeAccel) {
		std::vector<GameObject*>possibleObjs;
		CheckRayIntersectionWithGameObjectAndChildren(picking, possibleObjs, App->scene->GetRoot(), GetGameObjectSelected());

		if (possibleObjs.size() > 0) {

			std::sort(possibleObjs.begin(), possibleObjs.end(), [](GameObject* l, GameObject* r) {
				float3 frustumPosition = App->camera->GetFrustum().Pos();
				float lDist = (frustumPosition - l->GetAABB().CenterPoint()).Length();
				float rDist = (frustumPosition - r->GetAABB().CenterPoint()).Length();

				return lDist < rDist;
				});

			float3 frustumPosition = App->camera->GetFrustum().Pos();

			int id = 0;

			while (selected == nullptr && id < possibleObjs.size()) {
				if (possibleObjs[id] != App->scene->GetRoot()) {
					if (!PreviouslySelected(possibleObjs[id])) {
						if (CheckRayIntersectionWithMeshRenderer(picking, possibleObjs[id])) {
							selected = possibleObjs[id];
						}
					}
				}
				++id;
			}


			//std::reverse(possibleObjs.begin(), possibleObjs.end());
			//int id = 0;

			//while (selected == nullptr && id < possibleObjs.size()) {
			//	if (possibleObjs[id] != App->scene->GetRoot()) {
			//		if (hierarchy->GetGameObjectSelected() == possibleObjs[id]) {
			//			for (std::vector<GameObject*>::iterator it = possibleObjs.begin(); it != possibleObjs.end() && selected == nullptr; ++it) {
			//				if (CheckRayIntersectionWithMeshRenderer(picking, *it)) {
			//					selected = *it;
			//				}
			//			}
			//		} else if (!PreviouslySelected(possibleObjs[id])) {
			//			selected = possibleObjs[id];
			//		}
			//	}

			//	id++;
			//}

		}
	} else {
		if (useMultiMap) {
			std::multimap<float, GameObject*>possibleObjs;

			CheckRayIntersectionWithQuadTreeNode(picking, possibleObjs, *App->scene->GetQuadTree()->root, App->camera->GetFrustum().Pos());

			for (std::multimap<float, GameObject*>::iterator it = possibleObjs.begin(); it != possibleObjs.end() && selected == nullptr; ++it) {
				if (!PreviouslySelected((*it).second)) {
					if (CheckRayIntersectionWithMeshRenderer(picking, (*it).second)) {
						selected = (*it).second;
					}
				}
			}

		} else {
			std::vector<GameObject*>possibleObjs;
			CheckRayIntersectionWithQuadTreeNode(picking, possibleObjs, *App->scene->GetQuadTree()->root);

			if (possibleObjs.size() > 0) {

				std::sort(possibleObjs.begin(), possibleObjs.end(), [](GameObject* l, GameObject* r) {
					float3 frustumPosition = App->camera->GetFrustum().Pos();
					float lDist = (frustumPosition - l->GetAABB().CenterPoint()).Length();
					float rDist = (frustumPosition - r->GetAABB().CenterPoint()).Length();

					return lDist < rDist;
					});

				float3 frustumPosition = App->camera->GetFrustum().Pos();

				int id = 0;

				while (selected == nullptr && id < possibleObjs.size()) {
					if (possibleObjs[id] != App->scene->GetRoot()) {
						if (!PreviouslySelected(possibleObjs[id])) {
							if (CheckRayIntersectionWithMeshRenderer(picking, possibleObjs[id])) {
								selected = possibleObjs[id];
							}
						}
					}
					++id;
				}



			}

		}
	}
	return selected;
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
	PreciseTimer* timer = new PreciseTimer();
	timer->Start();
	selected = TryToSelectGameObject(picking, useQuadTreeAcceleration);

	LOG("Selection took %lf secs", timer->Read());

	delete timer;
	SetGameObjectSelected(selected);

	if (!selected) {
		previouslySelectedGameObjects.clear();
	} else {
		previouslySelectedGameObjects.push_back(selected);
	}
}

bool ModuleEditor::PreviouslySelected(GameObject* obj)const {
	bool found = false;
	for (std::vector<GameObject*>::const_iterator it = previouslySelectedGameObjects.begin(); it != previouslySelectedGameObjects.end() && !found; ++it) {
		if (obj == *it) {
			found = true;
		}
	}
	return found;
}



bool ModuleEditor::CheckRayIntersectionWithMeshRenderer(const LineSegment& picking, const GameObject* o) {
	ComponentMeshRenderer* mesh = (ComponentMeshRenderer*)o->GetComponentOfType(ComponentType::CTMeshRenderer);
	if (!mesh)return false;
	ComponentTransform* transform = (ComponentTransform*)o->GetComponentOfType(ComponentType::CTTransform);

	float4x4 model = transform->globalMatrix;

	LineSegment lineToUse = model.Inverted() * picking;

	std::vector<Triangle> tris = mesh->mesh->GetTriangles();

	bool intersection = false;

	for (std::vector<Triangle>::iterator it = tris.begin(); it != tris.end() && !intersection; ++it) {
		float dist;
		float3 point;
		if (lineToUse.Intersects(*it, &dist, &point)) {
			intersection = true;
		}
	}

	return intersection;
}


void ModuleEditor::CheckRayIntersectionWithGameObjectAndChildren(const LineSegment& ray, std::vector<GameObject*>& possibleAABBs, GameObject* o, const GameObject* currentSelected) {
	//Use multi map
	if (o != currentSelected) {
		AABB aabb = o->GetAABB();
		Ray realRay = ray.ToRay();

		if (realRay.Intersects(aabb)) {
			possibleAABBs.push_back(o);
		}
	}

	for (std::vector<GameObject*>::iterator it = o->children.begin(); it != o->children.end(); ++it) {
		CheckRayIntersectionWithGameObjectAndChildren(ray, possibleAABBs, *it, currentSelected);
	}

}

bool ModuleEditor::CheckRayIntersectionWithGameObject(const LineSegment& ray, GameObject* a, const GameObject* currentSelected) {
	if (a == currentSelected || a == App->scene->GetRoot())return false;
	if (ray.Intersects(a->GetAABB())) {
		return true;
	}
	return false;
}

void ModuleEditor::CheckRayIntersectionWithQuadTreeNode(const LineSegment& picking, std::vector<GameObject*>& possibleObjs, const QuadtreeNode& node) {
	if (picking.Intersects(node.boundingBox)) {
		for (std::vector<GameObject*>::const_iterator it = node.gameObjects.begin(); it != node.gameObjects.end(); ++it) {
			if (picking.Intersects((*it)->GetAABB())) {
				possibleObjs.push_back(*it);
			}
		}
		for (std::vector<QuadtreeNode>::const_iterator it = node.childNodes.begin(); it != node.childNodes.end(); ++it) {
			CheckRayIntersectionWithQuadTreeNode(picking, possibleObjs, *it);
		}
	}
}

void ModuleEditor::CheckRayIntersectionWithQuadTreeNode(const LineSegment& picking, std::multimap<float, GameObject*>& possibleObjs, const QuadtreeNode& node, const float3& frustumPos) {

	if (picking.Intersects(node.boundingBox)) {
		for (std::vector<GameObject*>::const_iterator it = node.gameObjects.begin(); it != node.gameObjects.end(); ++it) {
			if (picking.Intersects((*it)->GetAABB())) {
				possibleObjs.insert(std::pair<float, GameObject*>  { Length(frustumPos - (*it)->GetAABB().CenterPoint()), * it});
			}
		}
		for (std::vector<QuadtreeNode>::const_iterator it = node.childNodes.begin(); it != node.childNodes.end(); ++it) {
			CheckRayIntersectionWithQuadTreeNode(picking, possibleObjs, *it, frustumPos);
		}
	}
}

bool ModuleEditor::GetDrawQuadTree()const {
	return drawQuadTree;
}
void ModuleEditor::SetDrawQuadTree(bool should) {
	drawQuadTree = should;
}

//
//void ModuleEditor::CheckRayIntersectionWithQuadTreeNode(const LineSegment& picking, std::vector<QuadtreeNode>& possibleQTrees, QuadtreeNode& node) {
//	AABB aabb = node.boundingBox;
//	Ray realRay = picking.ToRay();
//
//	if (realRay.Intersects(aabb)) {
//		possibleQTrees.push_back(node);
//	}
//
//	if (node.subdivided) {
//		for (std::vector<QuadtreeNode>::iterator it = node.childNodes.begin(); it != node.childNodes.end(); ++it) {
//			CheckRayIntersectionWithQuadTreeNode(picking, possibleQTrees, *it);
//		}
//	}
//
//}