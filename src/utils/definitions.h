#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

enum PipelineType { Flat, Wire };

struct Model {
	const char* ObjFile;
	const char* TextureFile;
	vec3 pos;
	float scale;
	PipelineType pt;
	int id;
};

//Collections of points which collision will be tested
struct CollisionObject{
    std::vector<vec3> points = {};//For now contains only one point
    vec3 forceAfterCollision;
    bool isColliding =false;//default

};


/// <summary>
/// Component used ot hold physics values for entities
/// </summary>
struct RigidBody {
	vec3 pos;
	vec3 force;
	vec3 velocity;
	float mass;//Mass in kg

	vec3 fGravity;//Gravitational force, having a different force per object can allow different object to have different gravities
	bool hasGravity;//True if gravitational force is applied

	float staticFriction;
	float dynamicFriction;
	float bounciness;
	quat rot;

    CollisionObject* co;

};

struct Transform { //Describes an object location
    vec3 Position;
    vec3 Scale;
    quat Rotation;
};


//A volume that when point are in it generates collisions
struct Collider {
    //Function tha when given an object containing points
    // returns a force
    virtual void testCollision(CollisionObject* co);




};