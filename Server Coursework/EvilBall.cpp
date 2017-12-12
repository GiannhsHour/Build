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
	, maze_pos(Vector3(-1,-1, -1))
	, ac_time(0)
	, generate(false)

{
	state_name = "patrol";
	PhysicsEngine::Instance()->AddPhysicsObject(avatar);
}

EvilBall::~EvilBall()
{
}

//Generate a random path to use for patrol
void EvilBall::GeneratePath(bool random, const Vector3 player) {

	//clear everything
	enable_avatar = true;
	avatarIndex = 0;
	avatar_cellpos.clear();
	avatar_velocities.clear();

	int maze_size = generator->GetSize();
	SearchAStar* search_as = new SearchAStar();
	search_as->SetWeightings(1.0f, 1.0f);

	if (random) {
		
		if (!start) {
			int x = rand() % maze_size;
			int y = rand() % maze_size;

			int indexs = y * maze_size + x;

			start = &generator->allNodes[indexs];
		}
		else {
			if (first_path_after_chase) {
				start = &generator->allNodes[(int)(maze_pos.y * maze_size + maze_pos.x)];
			}
			else {
				start = end;
			}
		}

		int x = rand() % maze_size;
		int y = rand() % maze_size;

		int indexs = y * maze_size + x;
		end = &generator->allNodes[indexs];
	}
	else {
		start = &generator->allNodes[(int)(maze_pos.y * maze_size + maze_pos.x)];
		end = &generator->allNodes[(int)((int)player.y * maze_size + (int)player.x)];
	}

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

// line of sight based chase
bool EvilBall::isInLineOfSight(Vector3 player) {
	bool iSeeYou = false;
	int size = generator->GetSize();
	if (maze_pos.x == player.x) { 
		iSeeYou = true;
		int maxY, minY;
		if (maze_pos.y > player.y) {
			maxY = maze_pos.y;
			minY = player.y;
		}
		else {
			maxY = player.y;
			minY = maze_pos.y;
		}
		for (int i = minY; i < maxY; i++) {

			int index = size * (size - 1) + (maze_pos.x * (size - 1) + i);
			if (generator->allEdges[index]._iswall) {
				iSeeYou = false;
				break;
			}
		}

	}
	else if (maze_pos.y == player.y) {
		iSeeYou = true;
		int maxX, minX;
		if (maze_pos.x > player.x) {
			maxX = maze_pos.x;
			minX = player.x;
		}
		else {
			maxX = player.x;
			minX = maze_pos.x;
		}
		for (int i = minX; i < maxX; i++) {
			int index = (maze_pos.y * (size - 1) + i);
			if (generator->allEdges[index]._iswall) {
				iSeeYou = false;
				break;
			}
		}
	}
	return iSeeYou;
}

void EvilBall::Chase() {
	//clear everything
	ac_time += 1.0f;
	if ((chasing_player->cur_cell_pos - maze_pos).Length() > 4.0f) {
		first_path_after_chase = true;
		state_name = "patrol";
		reached_end = true;
		return;
	}
	cout << "Maze POS: " << maze_pos.x << " " << maze_pos.y << " " << maze_pos.z << endl;
	cout << "Actual POS: " << avatar->GetPosition().x << " " << avatar->GetPosition().y << " " << avatar->GetPosition().z << endl;
	if (generate) {
		GeneratePath(false, chasing_player->cur_cell_pos);
		ac_time = 0;
	}

	UpdatePosition();
	
}

void EvilBall::Patrol(vector<Player*>& players) {
	
	for (int i = 0; i < players.size(); i++) {
		if (players[i]->enable_avatar) {
			if ((players[i]->cur_cell_pos - maze_pos).Length() < 4.0f) {
				state_name = "chase";
				chasing_player = players[i];
				reached_end = true;
				return;
			}
		}
	//	Line of sight instead of radius
	/*	if (isInLineOfSight(player_pos[i])) {
			state_name = "chase";
			reached_end = true;
			return;
		}*/

	
	}

	if (reached_end) {
		GeneratePath();
		first_path_after_chase = false;
	}
	UpdatePosition();
}

void EvilBall::UpdatePosition() {
	generate = false;
	if (enable_avatar) {
		if (avatarIndex < avatar_velocities.size()) {
			avatar->SetLinearVelocity(avatar_velocities[avatarIndex] * 1.0f);
			float avatar_dist = (avatar->GetPosition() - avatar_cellpos[avatarIndex + 1]).Length();
			if (avatar_dist <= 0.05f) {
				avatar->SetPosition(avatar_cellpos[avatarIndex + 1]);
				avatarIndex++;
				generate = true;
			}
			position = id + " " + to_string(avatar->GetPosition().x) + " " +
				to_string(avatar->GetPosition().y) + " " +
				to_string(avatar->GetPosition().z) + " ";
			maze_pos = avatar_cellpos[avatarIndex];
			
		}
		else {
			avatar->SetLinearVelocity(Vector3(0, 0, 0));
			reached_end = true;
		} 
	}
}
