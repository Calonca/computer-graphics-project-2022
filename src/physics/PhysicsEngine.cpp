#include <numeric>
#include "PhysicsEngine.hpp"
#include "../gameObjects/Truck.hpp"

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


const float THRESHOLD = 0.002f;
float accumulator = 0.0f;

void PhysicsEngine::Step(float dt, GLFWwindow *window)
{
    accumulator +=dt;
    while (accumulator > THRESHOLD){
        ApplyGravity();
        SolveCollisions(window);
        ApplyForces(THRESHOLD);
        accumulator-=THRESHOLD;
    }
}

//Adds forces to rigidbodies based on collisions
void PhysicsEngine::SolveCollisions(GLFWwindow *window) {
    //For each collider check collision objects and set force
    for(TerrainCollider* collider : colliders){
        for (RigidBody* rb : rbs) {
            int numColliding = 0;
            vec3 rbNormal = vec3(0,0,0);

            for (CollisionObject* collisionObject : rb->co) {
                collisionObject->setTransform(rb);
                collider->testCollision(collisionObject);

                if (collisionObject->isColliding) {
                    numColliding += 1;
                    rbNormal += collisionObject->normal;
                }

            }

            for (CollisionObject* collisionObject : rb->co) {
                if (collisionObject->isColliding) {
                    vec3 force = collisionObject->forceAfterCollision;
                    rb->addGlobalMoment( force * rb->mass * (40.0f / numColliding),
                                         collisionObject->getLocalPoint(0));
                }
            }
            /*
            //MatrixUtils::printVector(normal);
            if (window!= nullptr && glfwGetKey(window, GLFW_KEY_J)) {
                rb ->addGlobalMoment(10000.0f*normal,
                                    vec3(0,0,0));
            }*/

            int maxCollider = rb->co.size();
            if (numColliding > maxCollider - 1) {
                glm::vec3 aVel = rb->velocity;
                glm::vec3 bVel = glm::vec3(0.0f);
                glm::vec3 rVel = bVel - aVel;
                vec3 normal = normalize(rbNormal);
                float nSpd = dot(rVel, normal);

                float aInvMass = 1 / rb->mass;

                // Impluse

                // This is important for convergence
                // a negitive impulse would drive the objects closer together
                if (nSpd < 0)
                    continue;

                float j = -(1.0f + rb->bounciness) * nSpd / (aInvMass);

                glm::vec3 impluse = j * normal;

                //MatrixUtils::printVector(normal);

                aVel -= impluse * aInvMass;
                //std::cout<<"Velocity delta after impulse: "<< MatrixUtils::printVector(-impluse * aInvMass)<< std::endl;

                // Friction

                rVel = bVel - aVel;
                //MatrixUtils::printVector(rVel);
                nSpd = glm::dot(rVel, normal);
                //std::cout<<"f is :"<<nSpd<<std::endl;

                glm::vec3 tangent = -rVel + nSpd * normal;
                //MatrixUtils::printVector(tangent);
                //std::cout<<"tan len is :"<<length(tangent)<<std::endl;

                if (length(tangent) > 0.0001f) { // safe normalize
                    tangent = glm::normalize(tangent);
                }


                //MatrixUtils::printVector(tangent);

                float f  = -dot(rVel, tangent) / (aInvMass );
                //std::cout<<"f is :"<<f<<std::endl;

                glm::vec3 friction;
                if (abs(f) < j * rb->staticFriction) {
                    friction = f * tangent;

                }

                else {
                    friction = -j * tangent * rb->dynamicFriction;
                    //std::cout<<"Dyn friction"<<std::endl;
                }

                rb->velocity = aVel - friction * aInvMass;

            }

        }
    }
}

void PhysicsEngine::ApplyGravity() {
    for (RigidBody* rb : rbs) {
        if (rb->hasGravity){
            rb->addGlobalMoment(rb->mass * rb->fGravity,vec3(0,-0.05,0));//Adds gravity to forces
        }
    }
}



void PhysicsEngine::ApplyForces(float dt) {
    //std::cout<<"deltatime is :"<<dt<<std::endl;
    float inertia = 700;
    for (RigidBody* rb : rbs) {
        mat4 transform = rb->parent->getTransform();

        vec3 resultingForce = vec3(0,0,0);
        vec3 torque = vec3(0,0,0);

        //std::cout<<"Vertos"<<std::endl;
        for (Moment moment : rb->moments){
            //std::cout<<"force: ";
            //MatrixUtils::printVector(moment.force);
            vec3 force;
            if (moment.isGlobal)
                force = MatrixUtils::fromGlobalToLocal(transform,moment.force);
            else
                force = moment.force;

            resultingForce += force;
            //std::cout<<"pos: ";
            //MatrixUtils::printVector(moment.point);
            torque += cross(moment.point,force);
        }

        rb->velocity += (resultingForce / rb->mass) * dt;

        vec3 angularAcceleration = torque/inertia;

        rb->angularVelocity += angularAcceleration*dt;

        //Angular drag
        const float angularDrag = 0.01f;
        rb->angularVelocity -= angularDrag * rb->angularVelocity;
        //Adds air friction
        vec3 diffVel = 0.0001f*(rb->velocity*rb->velocity* normalize(rb->velocity));
        diffVel.y =0;
        rb->velocity -= diffVel;

        //Rotation due to angular velocity
        transform = transform * rotate(mat4(1),rb->angularVelocity.r*dt,vec3(1,0,0));
        transform = transform * rotate(mat4(1),rb->angularVelocity.p*dt,vec3(0,0,1));
        transform = transform * rotate(mat4(1),rb->angularVelocity.y*dt,vec3(0,1,0));

        // Translate the Rigidbody based on the velocity in the Rigidbody reference system
        transform = transform * translate(mat4(1),rb->velocity*dt);
        rb->parent->setTransform(transform);

        //Reset values
        rb->moments = {};
        //rb->angularVelocity = 0;

    }
}
