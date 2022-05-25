#include "PhysicsEngine.h"
#include <iostream>


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
