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
	std::vector<QuadtreeNode> childNodes;
	std::vector<GameObject*> gameObjects;

private:

	QuadtreeNode* parent = nullptr;
	bool IsEmpty()const;
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

