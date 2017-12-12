
#pragma once
#include <ncltech\GameObject.h>
#include <functional>
#include <algorithm>
#include "MazeGenerator.h"
#include "SearchAStar.h"



class EvilBall : public GameObject
{
public:
	EvilBall(MazeGenerator* gen, string identity);
	~EvilBall();

	std::string GetStateName() { return state_name; }

	void SetMazePos(Vector3 pos) {avatar->SetPosition(pos); }
	Vector3 GetMazePos() { return avatar->GetPosition(); }

	void SetId(string identity) { id = identity; }
	string GetId() { return id; }

	string GetPositionAsString() { return position; }

	void EnableBall() { enable_avatar = true; }

	void GenerateRandomPath();

	//States
	void Patrol();
	void Chase();

protected:
	//Global
	std::list<const GraphNode*> patrol_path;
	GraphNode* start;
	GraphNode* end;

	std::string state_name;
	string position;
	string id;
	
	bool patrolling;

	MazeGenerator* generator;

	Vector3 maze_pos;
	Vector3 target_pos;

	vector<Vector3> avatar_velocities;
	vector<Vector3> avatar_cellpos;
	int avatarIndex;
	PhysicsNode* avatar;
	bool enable_avatar;
	string peer_id;
	
	bool reached_end;
	

};