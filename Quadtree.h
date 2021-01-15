#pragma once
#include "MathGeoLib/Geometry/AABB.h"
#include <list>

#define NODE_MAX_CAPACITY 2

class GameObject;

class QuadtreeNode {

public:
	QuadtreeNode(AABB boundingBox);
	~QuadtreeNode();

	void InsertGameObject(GameObject* gameObject);
	void EraseGameObject(GameObject* gameObject);

	void Draw()const;
	void Subdivide();
	void Organize();
	QuadtreeNode* GetParent() const { return parent; }

public:
	bool subdivided;
	AABB boundingBox;

	//QuadtreeNode* nw = nullptr;
	//QuadtreeNode* ne = nullptr;
	//QuadtreeNode* sw = nullptr;
	//QuadtreeNode* se = nullptr;

	std::vector<QuadtreeNode> childNodes;
	std::vector<GameObject*> gameObjects;

	//std::vector<GameObject*> gameObjects
	//gameObjects.reserve (NODE_MAX_CAPACITY);

private:

	QuadtreeNode* parent = nullptr;

};

class Quadtree {

public:
	Quadtree(const AABB& boundingBox);
	~Quadtree();

	void InsertGameObject(GameObject* gameObject);
	void EraseGameObject(GameObject* gameObject);

	void Draw();


public:
	QuadtreeNode* root = nullptr;

};

