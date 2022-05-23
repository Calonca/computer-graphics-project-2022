#include <glm/glm.hpp>
#include "definitions.h"
#include <vector>
#include "glm/gtc/matrix_transform.hpp"
#include <GLFW/glfw3.h>

extern struct Model g_test;

#pragma once
class Truck// It is an entity
{
public:
	// Robot Pos
	RigidBody rb = {
		vec3(3, 0, 2),   //pos
		vec3(0,0,0),     //force	
		vec3(0,0,0),     //velocity
		float(10.0f),    //mass
		vec3(0,0,0),     // fGravity
		false,           // hasGravity
		0.9,             // static friction
		0.2,             //dynamic friction
		0.0f,            //bounciness
		quat(vec3(0,glm::radians(-30.0f),0))
	};
	glm::vec3 RobotCamDeltaPos = glm::vec3(0.0f, 1.335f, -0.0f);
	glm::vec3 FollowerDeltaTarget = glm::vec3(0.0f, 1.335f, 0.0f);
	float followerDist = 1.8;
	float lookYaw = 0.0;
	float lookPitch = glm::radians(-30.0f);
	float lookRoll = 0.0;
	std::vector<Model> stl = { 
		{ "MonsterTruck/Truck.obj", "MonsterTruck/Truck.png", {0,0,0}, 1, Flat ,2}
	};

	const float ROT_SPEED = glm::radians(60.0f);
	const float MOVE_SPEED = 9000.75f;
	const float MOUSE_RES = 500.0f;
	
	void UpdatePos(GLFWwindow* window, float deltaT);

};

