#include <glm/glm.hpp>
#include "definitions.h"
#include <vector>
extern struct Model g_test;

#pragma once
class Truck
{
public:
	// Robot Pos
	glm::vec3 TruckPos = glm::vec3(3, 0, 2);
	glm::vec3 RobotCamDeltaPos = glm::vec3(0.0f, 1.335f, -0.0f);
	glm::vec3 FollowerDeltaTarget = glm::vec3(0.0f, 1.335f, 0.0f);
	float followerDist = 1.8;
	float lookYaw = 0.0;
	float lookPitch = glm::radians(-30.0f);
	float lookRoll = 0.0;
	std::vector<Model> stl = { 
		{ "MonsterTruck/Truck.obj", "MonsterTruck/Truck.png", {0,-10000.0f,0}, 1, Flat ,2}
	};
};

