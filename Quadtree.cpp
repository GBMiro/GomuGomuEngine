#include "Quadtree.h"
#include "debug_draw/debugdraw.h"
#include "Globals.h"
#include "GameObject.h"
#include "Leaks.h"

QuadtreeNode::QuadtreeNode(const AABB& boundingBox) : boundingBox(boundingBox)  {
}

QuadtreeNode::~QuadtreeNode() {
	RELEASE(nw);
	RELEASE(ne);
	RELEASE(sw);
	RELEASE(se);
}

void QuadtreeNode::InsertGameObject(GameObject* gameObject) {
	if (!nw) { //It's not subdivided
		gameObjects.push_back(gameObject);
		LOG("Game Object inserted");
		if (gameObjects.size() > NODE_MAX_CAPACITY) {
			Subdivide();
			Organize();
		}
	}
	else {
		// If intersects in other QuadTree insert gameObject
		AABB gameObjectAABB = gameObject->GetAABB().MinimalEnclosingAABB();
		if(nw->boundingBox.Intersects(gameObjectAABB)) nw->InsertGameObject(gameObject);
		if(ne->boundingBox.Intersects(gameObjectAABB)) ne->InsertGameObject(gameObject);
		if(sw->boundingBox.Intersects(gameObjectAABB)) sw->InsertGameObject(gameObject);
		if(se->boundingBox.Intersects(gameObjectAABB)) se->InsertGameObject(gameObject);
	}
}

void QuadtreeNode::Draw() {
	dd::aabb(boundingBox.minPoint, boundingBox.maxPoint, float3::one);
	if (nw) { // If pointer is valid, draw all children
		nw->Draw();
		ne->Draw();
		sw->Draw();
		se->Draw();
	}
}

void QuadtreeNode::Subdivide() {
	LOG("Node is full. We need to subdivide");
	AABB aaBB;
	float3 centerPoint = boundingBox.CenterPoint();
	float3 childSize = boundingBox.HalfSize();
	childSize.y *= 2;

	float3 childCenter;

	// North - West
	childCenter.x = centerPoint.x - childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z  + childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	nw = new QuadtreeNode(aaBB);
	nw->parent = this;

	// North - East
	childCenter.x = centerPoint.x + childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z + childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	ne = new QuadtreeNode(aaBB);
	ne->parent = this;

	// South - West
	childCenter.x = centerPoint.x - childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z - childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	sw = new QuadtreeNode(aaBB);
	sw->parent = this;

	// South - East
	childCenter.x = centerPoint.x + childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z - childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	se = new QuadtreeNode(aaBB);
	se->parent = this;
}

void QuadtreeNode::Organize() {
	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();) {
		GameObject* currentGO = *it;

		bool intersections[4];
		unsigned intersectionsFound = 0;
		AABB gameObjectAABB(currentGO->GetAABB().MinimalEnclosingAABB());

		intersections[0] = nw->boundingBox.Intersects(gameObjectAABB);
		intersectionsFound = intersections[0] ? intersectionsFound + 1 : intersectionsFound;
		intersections[1] = ne->boundingBox.Intersects(gameObjectAABB);
		intersectionsFound = intersections[1] ? intersectionsFound + 1 : intersectionsFound;
		intersections[2] = sw->boundingBox.Intersects(gameObjectAABB);
		intersectionsFound = intersections[2] ? intersectionsFound + 1 : intersectionsFound;
		intersections[3] = se->boundingBox.Intersects(gameObjectAABB);
		intersectionsFound = intersections[3] ? intersectionsFound + 1 : intersectionsFound;

		if (intersectionsFound == 4) {
			++it;
		}
		else {
			it = gameObjects.erase(it);
			if (intersections[0]) nw->InsertGameObject(currentGO);
			if (intersections[1]) ne->InsertGameObject(currentGO);
			if (intersections[2]) sw->InsertGameObject(currentGO);
			if (intersections[3]) se->InsertGameObject(currentGO);
		}
	}
}

Quadtree::Quadtree(const AABB& boundingBox) {
	root = new QuadtreeNode(boundingBox);
}

Quadtree::~Quadtree() {
	RELEASE(root);
}

void Quadtree::InsertGameObject(GameObject* gameObject) {
	if (root) {
		if (gameObject->GetAABB().Intersects(root->boundingBox)) {
			root->InsertGameObject(gameObject);
		}
	}
}

void Quadtree::Draw() {
	root->Draw();
}
