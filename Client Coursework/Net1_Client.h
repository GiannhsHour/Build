
#pragma once

#include <ncltech\Scene.h>
#include <ncltech\NetworkBase.h>
#include "MazeRenderer.h"
#include <sstream>

//Basic Network Example

class Net1_Client : public Scene
{
public:
	Net1_Client(const std::string& friendly_name);

	virtual void OnInitializeScene() override;
	virtual void OnCleanupScene() override;
	virtual void OnUpdateScene(float dt) override;


	void ProcessNetworkEvent(const ENetEvent& evnt);

protected:
	void SendDataToServer(string data);
	string extractId(enet_uint8* packet) {
		stringstream ss;
		for (int i = 0; i < 4; i++) {
			ss << packet[i];
		}
		string res;
		ss >> res;
		return res;
	}

	string extractData(enet_uint8* packet, int length) {
		stringstream ss;
		for (int i = 4; i < length; i++) {
			ss << packet[i];
		}
		string res;
		getline(ss, res);
		return res.substr(1);
	}

	GameObject* box;

	MazeRenderer * maze;

	int maze_size = 0;
	float maze_density = 0.0f;
	
	vector<Vector3> path_vec;
	bool draw_path;

	NetworkBase network;
	ENetPeer*	serverConnection;
	

};