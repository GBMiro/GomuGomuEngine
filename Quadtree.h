#pragma once
#include "MathGeoLib/Geometry/AABB.h"
#include <list>

#define NODE_MAX_CAPACITY 2

class GameObject;

class QuadtreeNode {

public:
	QuadtreeNode(const AABB& boundingBox);
	~QuadtreeNode();

	QuadtreeNode* GetParent() const { return parent; }

public:
	AABB boundingBox;

	QuadtreeNode* nw = nullptr;
	QuadtreeNode* ne = nullptr;
	QuadtreeNode* sw = nullptr;
	QuadtreeNode* se = nullptr;

	std::list<GameObject*> gameObjects;

private:

	QuadtreeNode* parent = nullptr;

};

class Quadtree {

public:
	Quadtree();
	~Quadtree();


public:
	QuadtreeNode* root = nullptr;

};

