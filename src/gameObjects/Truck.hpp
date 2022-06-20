#include "glm/glm.hpp"
#include "../utils/definitions.hpp"
#include <vector>
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"
#include "Object.hpp"

extern struct Model g_test;

static mat4 initialTransform = translate(mat4(1), vec3(100, 10, 0));

#pragma once
class Truck : public Object// It is an entity
{
private:

    static std::vector<vec3> addSidePoints(){
        std::vector<vec3> points;
        //Front and back
        for (float xPos = -0.3f;xPos < 0.3f;xPos +=0.1f) {
            points.emplace_back(xPos, 1, 1);
            points.emplace_back(xPos, 1, -1);
        }
        //Sides
        for (float zPos = -0.8f; zPos < 0.8f; zPos +=0.2f) {
            points.emplace_back(0.5, 1, zPos);
            points.emplace_back(-0.5, 1, zPos);
        }
        return points;
    };
    CollisionObject sidePoints = {
            addSidePoints(),
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelfl = {
            {vec3(-0.5,0,-1)},
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelfr = {
            {vec3(0.5,0,-1)},
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelbl = {
            {vec3(-0.5,0,1)},
            initialTransform,
            vec3(0,0,0),
            false
    };

    CollisionObject wheelbr = {
            {vec3(0.5,0,1)},
            initialTransform,
            vec3(0,0,0),
            false
    };
public:

	// Robot Pos
	RigidBody rb = {
            nullptr,   //pos
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
		0.0f,            //bounciness
        {&sidePoints,&wheelfl,&wheelfr,&wheelbl,&wheelbr},
            vec3(0,0,0)
	};


    vec3 thirdPersonCamDelta = vec3(0.0f, 2, 2.0f) ;
    mat4 firstPersonCamDelta = translate(mat4(1), vec3(0.0f, 1.335f, -0.0f));

	const float ROT_SPEED = 30000;
	const float MOVE_SPEED = 8000000.0f;
	const float MOUSE_RES = 500.0f;

    //Truck(const std::string &id, const Model &model, const mat4 &transform);
    Truck();

    void UpdatePos(GLFWwindow *window, float deltaT);

    float rotateWheels(float initalRotation, float targetRotation, float deltaTime);

    //mat4 getGlobalTransform() const override;
};

