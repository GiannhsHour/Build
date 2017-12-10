
/******************************************************************************
Class: Net1_Client
Implements:
Author: Pieran Marris <p.marris@newcastle.ac.uk> and YOU!
Description:

:README:
- In order to run this demo, we also need to run "Tuts_Network_Client" at the same time.
- To do this:-
	1. right click on the entire solution (top of the solution exporerer) and go to properties
	2. Go to Common Properties -> Statup Project
	3. Select Multiple Statup Projects
	4. Select 'Start with Debugging' for both "Tuts_Network_Client" and "Tuts_Network_Server"

- Now when you run the program it will build and run both projects at the same time. =]
- You can also optionally spawn more instances by right clicking on the specific project
and going to Debug->Start New Instance.




FOR MORE NETWORKING INFORMATION SEE "Tuts_Network_Client -> Net1_Client.h"



		(\_/)
		( '_')
	 /""""""""""""\=========     -----D
	/"""""""""""""""""""""""\
....\_@____@____@____@____@_/

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <enet\enet.h>
#include <nclgl\GameTimer.h>
#include <nclgl\Vector3.h>
#include <nclgl\common.h>
#include <ncltech\PhysicsNode.h>
#include <ncltech\NetworkBase.h>
#include "MazeGenerator.h"
#include "SearchAStar.h"
#include <sstream>

//Needed to get computer adapter IPv4 addresses via windows
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


#define SERVER_PORT 1234
#define UPDATE_TIMESTEP (1.0f / 30.0f) //send 30 position updates per second

NetworkBase server;
GameTimer timer;
float accum_time = 0.0f;
float rotation = 0.0f;

MazeGenerator	generator;
int maze_edges = 0;
bool* edges;
int maze_size = 0;
float maze_density = 0.0f;

std::list<const GraphNode*> final_path;
vector<Vector3> avatar_velocities;
vector<Vector3> avatar_cellpos;
int avatarIndex = 0;

PhysicsNode* avatar;
bool enable_avatar = false;



void Win32_PrintAllAdapterIPAddresses();

int onExit(int exitcode)
{
	server.Release();
	system("pause");
	exit(exitcode);
}

void BroadcastData(char* data) {
	ENetPacket* packet = enet_packet_create(data, strlen(data), 0);
	enet_host_broadcast(server.m_pNetwork, 0, packet);
}

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
	if (length > 4) {
		stringstream ss;

		for (int i = 4; i < length; i++) {
			ss << packet[i];
		}
		string res;
		getline(ss, res);
		return res.substr(1);
	}
	else return "";

}

void calculate_velocities() {
	for (std::list<const GraphNode*>::iterator it = final_path.begin(); it != final_path.end(); ) {
		Vector3 startp = (*it)->_pos;
		++it;
		if (it != final_path.end()) {
			Vector3 endp = (*it)->_pos;
			avatar_velocities.push_back(endp - startp);
		}
	}

	
}

int main(int arcg, char** argv)
{   
	srand((uint)time(NULL));
	
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}

	//Initialize Server on Port 1234, with a possible 32 clients connected at any time
	if (!server.Initialize(SERVER_PORT, 32))
	{
		fprintf(stderr, "An error occurred while trying to create an ENet server host.\n");
		onExit(EXIT_FAILURE);
	}

	printf("Server Initiated\n");

	//Initialise the PhysicsEngine
	PhysicsEngine::Instance();

	Win32_PrintAllAdapterIPAddresses();

	timer.GetTimedMS();
	while (true)
	{
		float dt = timer.GetTimedMS() * 0.001f;
		accum_time += dt;
		rotation += 0.5f * PI * dt;

		//Handle All Incoming Packets and Send any enqued packets
		server.ServiceNetwork(dt, [&](const ENetEvent& evnt)
		{   
			
			switch (evnt.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
				printf("-%d Client Connected\n", evnt.peer->incomingPeerID);
				break;

			case ENET_EVENT_TYPE_RECEIVE: 
			{

				printf("\t Received data from client %d. Data length: %d \n", evnt.peer->incomingPeerID, evnt.packet->dataLength);
				string id = extractId(evnt.packet->data);
				string data = extractData(evnt.packet->data, evnt.packet->dataLength);
				
				if (id == "INIT") {
					enable_avatar = false;
					stringstream ss;
					string vals;
					ss << data;
					ss >> vals;
					maze_size = stoi(vals);
					ss >> vals;
					maze_density = stof(vals);
					printf("\t Received maze size %d and density %f from client %d \n", maze_size, maze_density, evnt.peer->incomingPeerID);
					string send = id +" "+data;
					BroadcastData(&send[0]);

					printf("\t Generating Maze... \n");
				
					generator.Generate(maze_size, maze_density);
					maze_edges = maze_size * (maze_size - 1) * 2;
					edges = new bool[maze_edges];
					for (int i = 0; i < maze_edges; i++) {
						edges[i] = (generator.allEdges[i]._iswall);
					}

					printf("\t Maze Generated!.\n");
				}
				else if (id == "OOKK") {
					printf("\t Sending data to Clients!\n");
					string send = "MAZE ";
					for (int i = 0; i < maze_edges; i++) {
						send += to_string(edges[i]);
					}
					BroadcastData(&send[0]);
				}
				else if (id == "CRDS") {
					printf("\t Received coords for start and end point: %s\n", data.c_str());
					SearchAStar* search_as = new SearchAStar();
					search_as->SetWeightings(1.0f, 1.0f);
					stringstream ss;
					int indexs, indexe;
					int x, y;
					string coord;
					ss << data;
					for (int i = 0; i < 2; i++) {
						ss >> coord; x = stoi(coord);
						ss >> coord; y = stoi(coord); ss >> coord;
						if (i == 0) indexs = y % maze_size * maze_size + x;
						else indexe = y % maze_size * maze_size + x;
					}

					GraphNode* start = &generator.allNodes[indexs];
					GraphNode* end = &generator.allNodes[indexe];
					
					enable_avatar = true;
					avatarIndex = 0;
					avatar_cellpos.clear();
					avatar_velocities.clear();
					avatar = new PhysicsNode();
					avatar->SetPosition(start->_pos);
					PhysicsEngine::Instance()->AddPhysicsObject(avatar);

					search_as->FindBestPath(start, end);
					final_path = search_as->GetFinalPath();
					string send = "ROUT ";
					for (std::list<const GraphNode*>::iterator it = final_path.begin(); it != final_path.end(); it++) {
						send += to_string((*it)->_pos.x) + " " + to_string((*it)->_pos.y) + " " + to_string((*it)->_pos.z) + " ";
						avatar_cellpos.push_back((*it)->_pos);
					}
					printf("\t Broadcasting final graph to clients.");
					BroadcastData(&send[0]);

					calculate_velocities();
				}
				else
				{
					NCLERROR("Size of package received: %d", evnt.packet->dataLength);
					NCLERROR("Recieved Invalid Network Packet!");
				}
				enet_packet_destroy(evnt.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("- Client %d has disconnected.\n", evnt.peer->incomingPeerID);
				break;
			}
		});

		if (accum_time >= UPDATE_TIMESTEP) {
			accum_time = 0.0f;
			if (enable_avatar) {
				if (avatarIndex < avatar_velocities.size()) {
					avatar->SetLinearVelocity(avatar_velocities[avatarIndex]*2.0f);
					float avatar_dist = (avatar->GetPosition() - avatar_cellpos[avatarIndex + 1]).Length();
					if (avatar_dist <= 0.1f) {
						avatar->SetPosition(avatar_cellpos[avatarIndex + 1]);
						avatarIndex++;
					}
					string send = "POSI " + to_string(avatar->GetPosition().x) + " " + to_string(avatar->GetPosition().y) + " " + to_string(avatar->GetPosition().z);
					BroadcastData(&send[0]);
				}
			}
		}
		PhysicsEngine::Instance()->Update(dt);

		Sleep(0);
	}

	system("pause");
	server.Release();
}




//Yay Win32 code >.>
//  - Grabs a list of all network adapters on the computer and prints out all IPv4 addresses associated with them.
void Win32_PrintAllAdapterIPAddresses()
{
	//Initially allocate 5KB of memory to store all adapter info
	ULONG outBufLen = 5000;
	

	IP_ADAPTER_INFO* pAdapters = NULL;
	DWORD status = ERROR_BUFFER_OVERFLOW;

	//Keep attempting to fit all adapter info inside our buffer, allocating more memory if needed
	// Note: Will exit after 5 failed attempts, or not enough memory. Lets pray it never comes to this!
	for (int i = 0; i < 5 && (status == ERROR_BUFFER_OVERFLOW); i++)
	{
		pAdapters = (IP_ADAPTER_INFO *)malloc(outBufLen);
		if (pAdapters != NULL) {

			//Get Network Adapter Info
			status = GetAdaptersInfo(pAdapters, &outBufLen);

			// Increase memory pool if needed
			if (status == ERROR_BUFFER_OVERFLOW) {
				free(pAdapters);
				pAdapters = NULL;
			}
			else {
				break;
			}
		}
	}

	
	if (pAdapters != NULL)
	{
		//Iterate through all Network Adapters, and print all IPv4 addresses associated with them to the console
		// - Adapters here are stored as a linked list termenated with a NULL next-pointer
		IP_ADAPTER_INFO* cAdapter = &pAdapters[0];
		while (cAdapter != NULL)
		{
			IP_ADDR_STRING* cIpAddress = &cAdapter->IpAddressList;
			while (cIpAddress != NULL)
			{
				printf("\t - Listening for connections on %s:%u\n", cIpAddress->IpAddress.String, SERVER_PORT);
				cIpAddress = cIpAddress->Next;
			}
			cAdapter = cAdapter->Next;
		}

		free(pAdapters);
	}
	
}