#include "glm/glm.hpp"
#include "../utils/definitions.h"
#include <vector>
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"

extern struct Model g_test;

#pragma once
class Truck// It is an entity
{
public:

    mat4 transfrom =
            translate(mat4(1),vec3(0,2,0));//*
            //mat4(quat(vec3(0,glm::radians(-30.0f),0)));
    CollisionObject wheel1 = {
            {vec3(0,0,0)+vec3(transfrom[3])},
            transfrom,
            vec3(0,0,0),
            false
    };//There should be one for each wheel but for now just one

	// Robot Pos
	RigidBody rb = {
        transfrom,   //pos
		vec3(0,0,0),     //force
		vec3(0,0,0),     //velocity
		float(1000.0f),    //mass
		vec3(0,-9.18,0),     // fGravity
        true,           // hasGravity
		0,             // static friction
		0,             //dynamic friction
		0.0f,            //bounciness
		&wheel1,
        0
	};

	glm::vec3 RobotCamDeltaPos = glm::vec3(0.0f, 2+1.335f, -0.0f) ;
	glm::vec3 FollowerDeltaTarget = glm::vec3(0.0f, 1.335f+transfrom[3].y, 0.0f) ;
	float followerDist = 1.8;
	float lookYaw = 0.0;
	float lookPitch = glm::radians(-30.0f);
	float lookRoll = 0.0;
	std::vector<Model> modelToLoad = {
		{"MonsterTruck/Truck.obj", "MonsterTruck/Truck.png", vec3(transfrom[3]), 1, Flat , 2}
	};

	const float ROT_SPEED = glm::radians(60.0f);
	const float MOVE_SPEED = 9000.75f;
	const float MOUSE_RES = 500.0f;

    void UpdatePos(GLFWwindow *window, float deltaT);
};

