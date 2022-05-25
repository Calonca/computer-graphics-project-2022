#include "PhysicsEngine.h"
#include <iostream>



void PhysicsEngine::AddRigidBody(RigidBody* rb)
{
	if (!rb) return;
	rbs.push_back(rb);
}

void PhysicsEngine::AddCollider(Collider *c) {
    if (!c) return;
    colliders.push_back(c);
}

void PhysicsEngine::RemoveCollider(Collider *c) {
    if (!c) return;
    auto iterator = std::find(colliders.begin(), colliders.end(), c);
    if (iterator != colliders.end())
        colliders.erase(iterator);
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
    ApplyGravity();
    SolveCollisions();
    ApplyForces(dt);
}



//Adds forces to rigidbodies based on collisions
void PhysicsEngine::SolveCollisions() {
    //For each collider check collision objects and set force
    for(Collider* collider : colliders){
        for (RigidBody* rb : rbs) {
            if (!rb->co) continue;
            CollisionObject* collisionObject =rb->co;
            collider->testCollision(collisionObject);
            if (collisionObject->isColliding) {
                rb->force += collisionObject->forceAfterCollision;
                //std::cout<<"After applying forces: "<<to_string(rb->force)<< std::endl;
            }
        }
    }
}

void PhysicsEngine::ApplyGravity() {
    for (RigidBody* rb : rbs) {
        if (rb->hasGravity)
            rb->force += rb->mass * rb->fGravity;//Adds gravity to forces
    }
}

void PhysicsEngine::ApplyForces(float dt) {
    for (RigidBody* rb : rbs) {

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


