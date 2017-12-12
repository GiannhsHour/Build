#include "EvilBall.h"
#include <ncltech\Scene.h>


EvilBall::EvilBall(MazeGenerator* gen, string identity)
	: GameObject("evil")
	, generator(gen)
	, start(NULL)
	, end(NULL)
	, avatarIndex(0)
	, enable_avatar(false)
	, avatar(new PhysicsNode())
	, reached_end(false)
	, id(identity)

{
	state_name = "patrol";
	PhysicsEngine::Instance()->AddPhysicsObject(avatar);
}

EvilBall::~EvilBall()
{
}

//Generate a random path to use for patrol
void EvilBall::GenerateRandomPath() {

	//clear everything
	enable_avatar = true;
	avatarIndex = 0;
	avatar_cellpos.clear();
	avatar_velocities.clear();

	int maze_size = generator->GetSize();
	SearchAStar* search_as = new SearchAStar();
	search_as->SetWeightings(1.0f, 1.0f);

	if (!start) {
		int x = rand() % maze_size;
		int y = rand() % maze_size;

		int indexs = y * maze_size + x;
	
		start = &generator->allNodes[indexs];
	}
	else start = end;


	int x = rand() % maze_size;
	int y = rand() % maze_size;

	int indexs = y * maze_size + x;
	end = &generator->allNodes[indexs];

	search_as->FindBestPath(start, end);
	patrol_path = search_as->GetFinalPath();

	//copy positions
	for (std::list<const GraphNode*>::iterator it = patrol_path.begin(); it != patrol_path.end(); it++) {
		avatar_cellpos.push_back((*it)->_pos);
	}
	//calculate velocities
	for (std::list<const GraphNode*>::iterator it = patrol_path.begin(); it != patrol_path.end(); ) {
		Vector3 startp = (*it)->_pos;
		++it;
		if (it != patrol_path.end()) {
			Vector3 endp = (*it)->_pos;
			avatar_velocities.push_back(endp - startp);
		}
	}
	avatar->SetPosition(start->_pos);
	reached_end = false;
	delete search_as;
}

void EvilBall::Patrol() {
	if (reached_end) {
		GenerateRandomPath();
	}
	if (enable_avatar) {
		if (avatarIndex < avatar_velocities.size()) {
			avatar->SetLinearVelocity(avatar_velocities[avatarIndex] * 1.0f);
			float avatar_dist = (avatar->GetPosition() - avatar_cellpos[avatarIndex + 1]).Length();
			if (avatar_dist <= 0.05f) {
				avatar->SetPosition(avatar_cellpos[avatarIndex + 1]);
				avatarIndex++;
			}
			position = id + " " + to_string(avatar->GetPosition().x) + " " +
				to_string(avatar->GetPosition().y) + " " +
				to_string(avatar->GetPosition().z) + " ";
		}
		else {
			avatar->SetLinearVelocity(Vector3(0, 0, 0));
			reached_end = true;
		}
	}
}

