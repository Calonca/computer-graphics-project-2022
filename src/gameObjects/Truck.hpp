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


private:

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
        {&wheelfl,&wheelfr,&wheelbl,&wheelbr},
            vec3(0,0,0)
	};


    vec3 thirdPersonCamDelta = vec3(0.0f, 2, 2.0f) ;
    mat4 firstPersonCamDelta = translate(mat4(1), vec3(0.0f, initialTransform[3].y + 1.335f, -0.0f));

	const float ROT_SPEED = 30000;
	const float MOVE_SPEED = 8000000.0f;
	const float MOUSE_RES = 500.0f;

    //Truck(const std::string &id, const Model &model, const mat4 &transform);
    Truck();

    void UpdatePos(GLFWwindow *window, float deltaT);

    inline float rotateWheels(float initalRotation,float targetRotation) {
        if (abs(targetRotation-initalRotation) < 0.01) {
            return initalRotation;
        }

        float rot;
        if (targetRotation > initalRotation) {
            rot = initalRotation+5.0f;
        } else {
            rot = initalRotation-5.0f;
        }

        Object* wheelFLObj =  children[2];
        Object* wheelFRObj =  children[3];

        float height = 0.4f;
        float sideDist = 0.5f;
        float frontDist = 0.7f;
        mat4 rotate180Y = rotate(mat4(1), radians(180.0f), vec3(0, 1, 0));

        wheelFLObj->setTransform(
                translate(mat4(1),vec3(-sideDist, height, -frontDist))*
                rotate(mat4(1), radians(-rot), vec3(0, 1, 0))
                );
        wheelFRObj->setTransform(
                translate(mat4(1),vec3(sideDist, height, -frontDist))*
                rotate(mat4(1), radians(-rot+180), vec3(0, 1, 0))
                );
        return rot;
    }

    //mat4 getTransform() const override;
};

