#include "glm/glm.hpp"
#include "../utils/definitions.hpp"
#include <vector>
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"
#include "Object.hpp"

extern struct Model g_test;

static mat4 initialTransform = translate(mat4(1), vec3(0, 0, 0));

#pragma once
class Truck : public Object// It is an entity
{
public:


    CollisionObject wheelfl = {
            {vec3(-0.5,0,-1)+vec3(initialTransform[3])},
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelfr = {
            {vec3(0.5,0,-1)+vec3(initialTransform[3])},
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelbl = {
            {vec3(-0.5,0,1)+vec3(initialTransform[3])},
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelbr = {
            {vec3(0.5,0,1)+vec3(initialTransform[3])},
            initialTransform,
            vec3(0,0,0),
            false
    };

	// Robot Pos
	RigidBody rb = {
            initialTransform,   //pos
        //0.0f,
        //glm::radians(-30.0f),
        //0.0f,
        {},     //force
		vec3(0,0,0),     //velocity
		float(1000.0f),    //mass
		vec3(0,-9.18,0),     // fGravity
        true,           // hasGravity
		0.8,             // static friction
		0.001f,             //dynamic friction
		0.1f,            //bounciness
        {wheelfl,wheelfr,wheelbl,wheelbr},
            vec3(0,0,0)
	};

	glm::vec3 FollowerDeltaTarget = glm::vec3(0.0f, 1.335f + initialTransform[3].y, 0.0f) ;

    mat4 camDelta = translate(mat4(1),vec3(0.0f, initialTransform[3].y + 1.335f, -0.0f));

	const float ROT_SPEED = 500000;
	const float MOVE_SPEED = 3000000.75f;
	const float MOUSE_RES = 500.0f;

    Truck(const std::string &id, const Model &model, const mat4 &transform);
    Truck();


    void UpdatePos(GLFWwindow *window, float deltaT);
};

