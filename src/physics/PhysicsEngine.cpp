#include "PhysicsEngine.h"

void PhysicsEngine::AddRigidBody(RigidBody* rb)
{
	if (!rb) return;
	rbs.push_back(rb);
}

void PhysicsEngine::AddCollider(TerrainCollider* c) {
    if (!c) return;
    colliders.push_back(c);
}

void PhysicsEngine::RemoveCollider(TerrainCollider *c) {
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
    for(TerrainCollider* collider : colliders){
        for (RigidBody* rb : rbs) {
            if (!rb->co) continue;
            CollisionObject* collisionObject =rb->co;
            //PlaneCollider* pl = dynamic_cast<PlaneCollider*>(collider);
            collider->testCollision(collisionObject, rb->pos);
            if (collisionObject->isColliding) {

                glm::vec3 aVel = rb->velocity;
                glm::vec3 bVel = glm::vec3(0.0f);
                glm::vec3 rVel = bVel - aVel;
                vec3 up = vec3(0,1,0);
                float nSpd = dot(rVel,up);

                float aInvMass = 1/rb->mass;
                float bInvMass = 1/100000;

                // Impluse

                // This is important for convergence
                // a negitive impulse would drive the objects closer together
                if (nSpd<0)
                    continue;

                float j = -(1.0f + rb->bounciness) * nSpd / (aInvMass + bInvMass);

                glm::vec3 impluse = j * up;

                rb->velocity -= impluse * aInvMass;
                //std::cout<<"After applying impulse: "<< MatrixUtils::printVector(impluse)<< std::endl;

                // Friction
                /*
                rVel = bVel - aVel;
                nSpd = glm::dot(rVel, manifold.Normal);

                glm::vec3 tangent = rVel - nSpd * manifold.Normal;

                if (glm::length(tangent) > 0.0001f) { // safe normalize
                    tangent = glm::normalize(tangent);
                }

                scalar fVel = glm::dot(rVel, tangent);

                scalar aSF = aBody ? aBody->StaticFriction  : 0.0f;
                scalar bSF = bBody ? bBody->StaticFriction  : 0.0f;
                scalar aDF = aBody ? aBody->DynamicFriction : 0.0f;
                scalar bDF = bBody ? bBody->DynamicFriction : 0.0f;
                scalar mu  = (scalar)glm::vec2(aSF, bSF).length();

                scalar f  = -fVel / (aInvMass + bInvMass);

                glm::vec3 friction;
                if (abs(f) < j * mu) {
                    friction = f * tangent;
                }

                else {
                    mu = glm::length(glm::vec2(aDF, bDF));
                    friction = -j * tangent * mu;
                }

                if (aBody ? aBody->IsSimulated : false) {
                    aBody->Velocity = aVel - friction * aInvMass;
                }

                if (bBody ? bBody->IsSimulated : false) {
                    bBody->Velocity = bVel + friction * bInvMass;
                }
                */
                rb->force +=  collisionObject->forceAfterCollision*50000.0f;

                std::cout<<"After applying forces: "<< MatrixUtils::printVector(rb->force)<< std::endl;
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


