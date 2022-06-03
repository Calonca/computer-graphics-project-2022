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
            translate(mat4(1),vec3(0,5,0));//*
            //mat4(quat(vec3(0,glm::radians(-30.0f),0)));
    CollisionObject wheelfl = {
            {vec3(-1,0,-1)+vec3(transfrom[3])},
            transfrom,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelfr = {
            {vec3(1,0,-1)+vec3(transfrom[3])},
            transfrom,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelbl = {
            {vec3(-1,0,1)+vec3(transfrom[3])},
            transfrom,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelbr = {
            {vec3(1,0,1)+vec3(transfrom[3])},
            transfrom,
            vec3(0,0,0),
            false
    };

	// Robot Pos
	RigidBody rb = {
        transfrom,   //pos
        //0.0f,
        //glm::radians(-30.0f),
        //0.0f,
        {},     //force
		vec3(0,0,0),     //velocity
		float(1000.0f),    //mass
		vec3(0,-9.18,0),     // fGravity
        true,           // hasGravity
		0,             // static friction
		0,             //dynamic friction
		0.0f,            //bounciness
        {wheelfl,wheelfr,wheelbl,wheelbr},
        vec3(0,0,0)
	};

	glm::vec3 FollowerDeltaTarget = glm::vec3(0.0f, 1.335f+transfrom[3].y, 0.0f) ;

    mat4 camDelta = translate(mat4(1),vec3(0.0f, transfrom[3].y+1.335f, -0.0f));
	std::vector<Model> modelToLoad = {
		{"MonsterTruck/Truck.obj", "MonsterTruck/Truck.png", vec3(transfrom[3]), 1, Flat , 2}
	};

	const float ROT_SPEED = 50000;
	const float MOVE_SPEED = 9000.75f;
	const float MOUSE_RES = 500.0f;

    void UpdatePos(GLFWwindow *window, float deltaT);
};

