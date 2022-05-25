#include "PhysicsEngine.h"
#include <iostream>

struct CollisionPoints {
    vec3 A; // Furthest point of A into B
    vec3 B; // Furthest point of B into A
    vec3 Normal; // B – A normalized
    float Depth;    // Length of B – A
    bool HasCollision;
};

struct Transform { // Describes an objects location
    vec3 Position;
    vec3 Scale;
    quat Rotation;
};

//Collections of points which collision will be tested
struct CollisionObject{
    std::vector<vec3> points = {};//For now contains only one point
    vec3 forceAfterCollision;
    bool isColliding;
};

//A volume that when point are in it generates collisions
struct Collider {
    //Function given an object containing points
    // returns a force
};

void PhysicsEngine::AddRigidBody(RigidBody* rb)
{
	if (!rb) return;
	rbs.push_back(rb);
}

void PhysicsEngine::RemoveRigidBody(RigidBody* rb)
{
	if (!rb) return;
	auto iterator = std::find(rbs.begin(), rbs.end(), rb);
	if (iterator != rbs.end())
		rbs.erase(iterator);
}



void PhysicsEngine::Step(float dt)
{

	for (RigidBody* rb : rbs) {

		if (rb->hasGravity)
			rb->force += rb->mass * rb->fGravity;//Adds gravity to forces

		//Adds drags, for now dynamic friction is computed as air drag and
		//static driction is computed as a force that stops sideways motion

		vec3 v = rb->velocity;
		rb->force -=  rb->dynamicFriction*v
			* rb->mass //To approximate area
			*
			(v*v);//Component wise multiplication

		//std::cout << length(rb->force) <<std::endl;
		
		//vec3 fwd = mat4(rb->rot) * glm::vec4(0, 0, -1, 1);

		rb->velocity += (rb->force / rb->mass) * dt;
		rb->pos += rb->velocity * dt;

		rb->force = vec3(0, 0, 0);
	
	}
}

void PhysicsEngine::SolveCollisions() {
    // for each collider check collision objects and set force

}
