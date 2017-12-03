#pragma once
#include <ncltech\AABB.h>
#include <ncltech\CollisionShape.h>
#include <ncltech\PhysicsNode.h>
#include <nclgl\NCLDebug.h>
#include <vector>


class OcTree {
public:
	OcTree(AABB* bnd) { boundary = bnd; }
	~OcTree() { 
		objectsIn.clear();
		children.clear();
		delete boundary;
	}

	float getHalfDim() { return boundary->halfdim; }
	Vector3 getCenter() { return boundary->center; }

	bool insert(PhysicsNode* p);
	 	
	std::vector<OcTree*> children;

	static void populateLeaves(OcTree* root);

	static void deleteTree(OcTree* root);

	static void draw(OcTree* root);

	static std::vector<OcTree*> leaves;

	std::vector<PhysicsNode*> objectsIn;


private:

	void subdivide();

	bool isLeaf() { return children.size() == 0; }

	AABB* boundary;

	

	static const int capacity = 10;
};
 
