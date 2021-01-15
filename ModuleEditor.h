#pragma once
#include "Module.h"
#include "Globals.h"
#include "Window.h"
#include <vector>
#include "imgui.h"
#include "ImGuizmo.h"
#include "MathGeoLib/Geometry/LineSegment.h"
#include "ModuleScene.h"
#include <map>
class WindowMonitor;
class WindowConfiguration;
class WindowConsole;
class WindowProperties;
class WindowAbout;
class WindowGameObjectHierarchy;
class WindowScene;
class GameObject;

class Quadtree;
class QuadtreeNode;

enum GameState {
	STOP,
	PAUSE,
	PLAY
};


class ModuleEditor : public Module {
private:
	ImGuizmo::OPERATION gizmoOperation;
	ImGuizmo::MODE gizmoMode;
	bool useQuadTreeAcceleration;
	std::vector<GameObject*>previouslySelectedGameObjects;
	std::vector<std::pair<float3, float3>> distances;
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();

	update_status PreUpdate();

	update_status Update();

	update_status PostUpdate();

	bool CleanUp();

	void registerFPS(float deltaTime) const;
	void registerLog(const char* log) const;
	void cleanProperties() const;
	void fileDropped(const char* filename) const;
	void SetGameWindowStatus(bool state) { gameWindowSelectedOrHovered = state; }
	bool GetGameWindowStatus() const { return gameWindowSelectedOrHovered; }

	GameObject* GetGameObjectSelected() const;
	void SetGameObjectSelected(GameObject* gameObject);
	GameObject* TryToSelectGameObject(const LineSegment& segment, bool quadTreeAccel = true);
	bool UseQuadTreeAcceleration()const;
	bool PreviouslySelected(GameObject* obj)const;
	void SetUseQuadTreeAcceleration(bool should);

	void SetSceneToLoad(SceneType type) { sceneToLoad = type; }
	SceneType GetSceneToLoad() const { return sceneToLoad; }
private:
	void CheckRayIntersectionWithGameObjectAndChildren(const LineSegment& ray, std::vector<GameObject*>& possibleAABBs, GameObject* o, const GameObject* currentSelected);
	bool CheckRayIntersectionWithGameObject(const LineSegment& ray, GameObject* a, const GameObject* currentSelected);
	bool CheckRayIntersectionWithMeshRenderer(const LineSegment& picking, const GameObject* o);
	//void CheckRayIntersectionWithQuadTreeNode(const LineSegment& picking, std::vector<QuadtreeNode*>& possibleQTrees, QuadtreeNode* node);

	void CheckRayIntersectionWithQuadTreeNode(const LineSegment& picking, std::vector<QuadtreeNode>& possibleQTrees, QuadtreeNode& node);
	void CheckRayIntersectionWithQuadTreeNode(const LineSegment& picking, std::multimap<float, GameObject*>& possibleObjs, QuadtreeNode& node, const float3& frustumPos);

public:
	WindowMonitor* monitor = nullptr;
	WindowConfiguration* configuration = nullptr;
	WindowConsole* console = nullptr;
	WindowProperties* properties = nullptr;
	WindowAbout* about = nullptr;
	WindowGameObjectHierarchy* hierarchy = nullptr;
	WindowScene* game = nullptr;
public:
	void ManageGizmos();
	void OnMouseMotion();
private:
	update_status showMainMenu();
	void Draw();
	std::vector<Window*> windows;
	bool gameWindowSelectedOrHovered = false;
	void OnClicked(ImVec2 mousePosInScene);
	SceneType sceneToLoad = DEFAULT_SCENE;
	GameState state = STOP;
};

