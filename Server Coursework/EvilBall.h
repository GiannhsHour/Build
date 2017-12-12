
#pragma once
#include <ncltech\GameObject.h>
#include <functional>
#include <algorithm>
#include "MazeGenerator.h"
#include "SearchAStar.h"



class EvilBall : public GameObject
{
public:
	EvilBall();
	~EvilBall();

	std::string GetStateName() { return state_name; }

	void SetMazePos(Vector3 pos) { maze_pos = pos; }
	Vector3 GetMazePos() { return maze_pos; }

	void GenerateRandomPath(MazeGenerator* g);

	//States

	void Patrol(float dt);
	void Chase(float dt);

protected:
	//Global
	std::list<const GraphNode*> patrol_path;

	std::string state_name;
	
	bool patrolling;

	Vector3 maze_pos;
	Vector3 target_pos;

};