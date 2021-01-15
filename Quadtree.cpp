#include "Quadtree.h"
#include "debug_draw/debugdraw.h"
#include "Globals.h"
#include "GameObject.h"
#include "Leaks.h"

QuadtreeNode::QuadtreeNode(AABB boundingBox) : boundingBox(boundingBox), subdivided(false) {
}

QuadtreeNode::~QuadtreeNode() {
	//RELEASE(nw);
	//RELEASE(ne);
	//RELEASE(sw);
	//RELEASE(se);
}

void QuadtreeNode::InsertGameObject(GameObject* gameObject) {
	if (!subdivided) { //It's not subdivided
		gameObjects.push_back(gameObject);
		if (gameObjects.size() > NODE_MAX_CAPACITY) {
			Subdivide();
			Organize();
		}
	} else {
		// If intersects in other QuadTree insert gameObject
		AABB gameObjectAABB = gameObject->GetAABB().MinimalEnclosingAABB();


		for (std::vector<QuadtreeNode>::iterator it = childNodes.begin(); it != childNodes.end(); ++it) {
			if ((*it).boundingBox.Intersects(gameObjectAABB)) {
				(*it).InsertGameObject(gameObject);
			}
		}

		/*	if (nw->boundingBox.Intersects(gameObjectAABB)) nw->InsertGameObject(gameObject);
			if (ne->boundingBox.Intersects(gameObjectAABB)) ne->InsertGameObject(gameObject);
			if (sw->boundingBox.Intersects(gameObjectAABB)) sw->InsertGameObject(gameObject);
			if (se->boundingBox.Intersects(gameObjectAABB)) se->InsertGameObject(gameObject);*/
	}
}

// Right now not using it because I make the quadtree from scratch every frame. Inefficient
void QuadtreeNode::EraseGameObject(GameObject* gameObject) {
	//std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);

	std::vector<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);

	if (it != gameObjects.end()) { // Game Object found in node
		gameObjects.erase(it);
	}

	if (subdivided) { // If node has children check in every node for the object
		for (std::vector<QuadtreeNode>::iterator it2 = childNodes.begin(); it2 != childNodes.end(); ++it2) {
			(*it2).EraseGameObject(*it);
		}
	}


}

void QuadtreeNode::Draw() const {
	dd::aabb(boundingBox.minPoint, boundingBox.maxPoint, float3::one);
	if (subdivided) { // If pointer is valid, draw all children
		for (std::vector<QuadtreeNode>::const_iterator it = childNodes.begin(); it != childNodes.end(); ++it) {
			(*it).Draw();
		}
	}
}

void QuadtreeNode::Subdivide() {
	//AABB aaBB;
	float3 centerPoint = boundingBox.CenterPoint();
	float3 childSize = boundingBox.HalfSize();
	childSize.y *= 2;

	//float3 childCenter;

	//// North - West
	//childCenter.x = centerPoint.x - childSize.x / 2;
	//childCenter.y = centerPoint.y;
	//childCenter.z = centerPoint.z + childSize.z / 2;
	//aaBB.SetFromCenterAndSize(childCenter, childSize);
	//nw = new QuadtreeNode(aaBB);
	//nw->parent = this;

	//// North - East
	//childCenter.x = centerPoint.x + childSize.x / 2;
	//childCenter.y = centerPoint.y;
	//childCenter.z = centerPoint.z + childSize.z / 2;
	//aaBB.SetFromCenterAndSize(childCenter, childSize);
	//ne = new QuadtreeNode(aaBB);
	//ne->parent = this;

	//// South - West
	//childCenter.x = centerPoint.x - childSize.x / 2;
	//childCenter.y = centerPoint.y;
	//childCenter.z = centerPoint.z - childSize.z / 2;
	//aaBB.SetFromCenterAndSize(childCenter, childSize);
	//sw = new QuadtreeNode(aaBB);
	//sw->parent = this;

	//// South - East
	//childCenter.x = centerPoint.x + childSize.x / 2;
	//childCenter.y = centerPoint.y;
	//childCenter.z = centerPoint.z - childSize.z / 2;
	//aaBB.SetFromCenterAndSize(childCenter, childSize);
	//se = new QuadtreeNode(aaBB);
	//se->parent = this;


	//TO DO 

	childNodes.reserve(4);

	AABB aaBB;
	float3 childCenter;
	childCenter.x = centerPoint.x - childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z + childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	childNodes.push_back(QuadtreeNode(aaBB));

	childCenter.x = centerPoint.x + childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z + childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	childNodes.push_back(QuadtreeNode(aaBB));

	// South - West
	childCenter.x = centerPoint.x - childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z - childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	childNodes.push_back(QuadtreeNode(aaBB));


	// South - East
	childCenter.x = centerPoint.x + childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z - childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	childNodes.push_back(QuadtreeNode(aaBB));


	for (int i = 0; i < 4; i++) {
		childNodes[i].parent = this;
	}



	subdivided = true;
}

void QuadtreeNode::Organize() {
	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();) {
		GameObject* currentGO = *it;

		bool intersections[4];
		unsigned intersectionsFound = 0;
		AABB gameObjectAABB(currentGO->GetAABB().MinimalEnclosingAABB());

		for (int i = 0; i < 4; i++) {
			intersections[i] = childNodes[i].boundingBox.Intersects(gameObjectAABB);
			intersectionsFound = intersections[0] ? intersectionsFound + 1 : intersectionsFound;
		}

		/*	intersections[0] = nw->boundingBox.Intersects(gameObjectAABB);
			intersectionsFound = intersections[0] ? intersectionsFound + 1 : intersectionsFound;
			intersections[1] = ne->boundingBox.Intersects(gameObjectAABB);
			intersectionsFound = intersections[1] ? intersectionsFound + 1 : intersectionsFound;
			intersections[2] = sw->boundingBox.Intersects(gameObjectAABB);
			intersectionsFound = intersections[2] ? intersectionsFound + 1 : intersectionsFound;
			intersections[3] = se->boundingBox.Intersects(gameObjectAABB);
			intersectionsFound = intersections[3] ? intersectionsFound + 1 : intersectionsFound;*/

		if (intersectionsFound == 4) {
			++it;
		} else {
			it = gameObjects.erase(it);
			for (int i = 0; i < 4; i++) {
				if (intersections[i]) {
					childNodes[i].InsertGameObject(currentGO);
				}
			}
			/*if (intersections[0]) nw->InsertGameObject(currentGO);
			if (intersections[1]) ne->InsertGameObject(currentGO);
			if (intersections[2]) sw->InsertGameObject(currentGO);
			if (intersections[3]) se->InsertGameObject(currentGO);*/
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

void Quadtree::EraseGameObject(GameObject* gameObject) {
	if (root) {
		root->EraseGameObject(gameObject);
	}
}

void Quadtree::Draw() {
	root->Draw();
}
