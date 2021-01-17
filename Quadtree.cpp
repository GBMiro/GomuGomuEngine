#include "Quadtree.h"
#include "debug_draw/debugdraw.h"
#include "Globals.h"
#include "GameObject.h"
#include "Leaks.h"

QuadtreeNode::QuadtreeNode(AABB boundingBox) : boundingBox(boundingBox), subdivided(false) {
}

QuadtreeNode::~QuadtreeNode() {

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
	}
}

void QuadtreeNode::EraseGameObject(GameObject* gameObject) {
	std::vector<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);

	if (it != gameObjects.end()) { // Game Object found in node
		gameObjects.erase(it);
	}

	if (subdivided) {
		for (int i = 0; i < childNodes.size(); ++i) {
			childNodes[i].EraseGameObject(gameObject);
		}
	}

	//This could be optimized by collecting the AMOUNT of gameobjects contained within children, if children contain less than the MAX_CAPACITY, then their gameObjects would
	//Come back to the parent and they would be deleted
	bool allEmpty = true;
	for (int i = 0; i < childNodes.size() && allEmpty; ++i) {
		allEmpty = (childNodes[i].IsEmpty());
	}

	if (allEmpty) {
		childNodes.clear();
		subdivided = false;
	}
}

bool QuadtreeNode::IsEmpty() const {
	bool allEmpty = gameObjects.size() == 0;

	if (subdivided) {
		for (std::vector<QuadtreeNode>::const_iterator it = childNodes.begin(); it != childNodes.end() && allEmpty; ++it) {
			allEmpty = (*it).IsEmpty();
		}
	}
	return allEmpty;
}

void QuadtreeNode::Draw() const {
	dd::aabb(boundingBox.minPoint, boundingBox.maxPoint, float3::one);
	if (subdivided) { // If subdivided, draw all children
		for (std::vector<QuadtreeNode>::const_iterator it = childNodes.begin(); it != childNodes.end(); ++it) {
			(*it).Draw();
		}
	}
}

void QuadtreeNode::Subdivide() {
	float3 centerPoint = boundingBox.CenterPoint();
	float3 childSize = boundingBox.HalfSize();
	childSize.y *= 2;

	childNodes.reserve(CHILD_AMOUNT);

	AABB aaBB;
	float3 childCenter;

	//// North - West
	childCenter.x = centerPoint.x - childSize.x / 2;
	childCenter.y = centerPoint.y;
	childCenter.z = centerPoint.z + childSize.z / 2;
	aaBB.SetFromCenterAndSize(childCenter, childSize);
	childNodes.push_back(QuadtreeNode(aaBB));

	// North - East
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


	for (int i = 0; i < CHILD_AMOUNT; i++) {
		childNodes[i].parent = this;
	}

	subdivided = true;
}

/// <summary>
/// We split contained GameObjects among all child nodes
/// </summary>
void QuadtreeNode::Organize() {
	for (std::vector<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();) {
		GameObject* currentGO = *it;

		bool intersections[CHILD_AMOUNT];
		unsigned intersectionsFound = 0;
		AABB gameObjectAABB(currentGO->GetAABB().MinimalEnclosingAABB());

		for (int i = 0; i < CHILD_AMOUNT; i++) {
			intersections[i] = childNodes[i].boundingBox.Intersects(gameObjectAABB);
			intersectionsFound = intersections[0] ? intersectionsFound + 1 : intersectionsFound;
		}

		if (intersectionsFound > 1) {
			++it;
		} else {
			it = gameObjects.erase(it);
			for (int i = 0; i < CHILD_AMOUNT; i++) {
				if (intersections[i]) {
					childNodes[i].InsertGameObject(currentGO);
				}
			}
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
