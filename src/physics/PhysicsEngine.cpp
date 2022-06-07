#include <numeric>
#include "PhysicsEngine.hpp"

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

void PhysicsEngine::Step(float dt)
{
    accumulator +=dt;
    while (accumulator > THRESHOLD){
        ApplyGravity();
        SolveCollisions();
        ApplyForces(THRESHOLD);
        accumulator-=THRESHOLD;
    }


}



//Adds forces to rigidbodies based on collisions
void PhysicsEngine::SolveCollisions() {
    //For each collider check collision objects and set force

    //std::cout<<"Testing collison points:"<<std::endl;
    for(TerrainCollider* collider : colliders){
        for (RigidBody* rb : rbs) {
            for (CollisionObject collisionObject : rb->co) {
                collisionObject.setTransform(rb);
                //PlaneCollider* pl = dynamic_cast<PlaneCollider*>(collider);
                collider->testCollision(&collisionObject);
                //Follow orientation
                /*
                vec4 forwardDir = rotate(mat4(1), radians(-90.0f), vec3(1, 0, 0))
                                  *
                                  vec4(collisionObject.normal, 1);

                vec3 pos = rb->transform[3];
                mat4 rotMat = translate(rb->transform, -pos);
                vec3 norm2 = rotMat * vec4(collisionObject.normal, 1);//Normal in car reference system

                glm::vec3 upVec(0, 1, 0);

                vec3 oldUp = normalize(vec3(rotMat * vec4(0, 1, 0, 1)));
                vec3 axis = cross(oldUp, norm2);
                float angle = acos(dot(oldUp, norm2));


                float xRot = 20.0f;


                //rb->transform = translate(mat4(1),pos)*
                //      MatrixUtils::LookAtMat(vec3(0,0,0),vec3(forwardDir),collisionObject->normal);
                vec3 euler = eulerAngles(quat_cast(rotMat));
                */
                //rb->transform = rotate(rb->transform,radians(xRot)-euler.x,vec3(1,0,0));

                if (collisionObject.isColliding) {
                    //rb->force += normalize(collisionObject->forceAfterCollision)*rb->mass*50000.0f*dt;
                    rb->addGlobalMoment(collisionObject.forceAfterCollision * rb->mass * 15.0f,collisionObject.getLocalPoint(0));

                    glm::vec3 aVel = rb->velocity;
                    glm::vec3 bVel = glm::vec3(0.0f);
                    glm::vec3 rVel = bVel - aVel;
                    vec3 normal = vec3(0,1,0);
                    float nSpd = dot(rVel, normal);

                    float aInvMass = 1 / rb->mass;

                    // Impluse

                    // This is important for convergence
                    // a negitive impulse would drive the objects closer together
                    if (nSpd < 0)
                        continue;

                    float j = -(1.0f + rb->bounciness) * nSpd / (aInvMass);

                    glm::vec3 impluse = j * normal;

                    aVel -= impluse * aInvMass*0.005f;
                    //std::cout<<"Velocity delta after impulse: "<< MatrixUtils::printVector(-impluse * aInvMass)<< std::endl;

                    // Friction
                    /*
                    rVel = bVel - aVel;
                    nSpd = glm::dot(rVel, normal);

                    glm::vec3 tangent = -rVel + nSpd * normal;

                    if (glm::length(tangent) > 0.0001f) { // safe normalize
                        tangent = glm::normalize(tangent);
                    }


                    //MatrixUtils::printVector(tangent);

                    float f  = -dot(rVel, tangent) / (aInvMass );

                    glm::vec3 friction;
                    if (abs(f) < j * rb->staticFriction) {
                        friction = f * tangent;
                    }

                    else {
                        friction = -j * tangent * rb->dynamicFriction;
                        friction = vec3(0,0,0);
                    }
                    friction = -j * tangent * rb->dynamicFriction;*/

                    rb->velocity = aVel;// - friction * aInvMass;

                    //std::cout<<"Impulse force: "<< MatrixUtils::printVector(collisionObject->forceAfterCollision*50000.0f*dt)<< std::endl;
                }
            }
        }
    }
}

void PhysicsEngine::ApplyGravity() {
    for (RigidBody* rb : rbs) {
        if (rb->hasGravity){
            rb->addGlobalMoment(rb->mass * rb->fGravity,vec3(0,-0.1,0));//Adds gravity to forces
        }
    }
}



void PhysicsEngine::ApplyForces(float dt) {
    //std::cout<<"deltatime is :"<<dt<<std::endl;
    float inertia = 1000;
    for (RigidBody* rb : rbs) {

        vec3 resultingForce = vec3(0,0,0);
        vec3 torque = vec3(0,0,0);

        //std::cout<<"Vertos"<<std::endl;
        for (Moment moment : rb->moments){
            //std::cout<<"force: ";
            //MatrixUtils::printVector(moment.force);
            vec3 force;
            if (moment.isGlobal)
                force = MatrixUtils::fromGlobalToLocal(rb->transform,moment.force);
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
        rb->velocity -= rb->dynamicFriction*rb->velocity;

        //Rotation due to angular velocity
        rb->transform = rb->transform * rotate(mat4(1),rb->angularVelocity.r*dt,vec3(1,0,0));
        rb->transform = rb->transform * rotate(mat4(1),rb->angularVelocity.p*dt,vec3(0,0,1));
        rb->transform = rb->transform * rotate(mat4(1),rb->angularVelocity.y*dt,vec3(0,1,0));

        // Translate the Rigidbody based on the velocity in the Rigidbody reference system
        rb->transform = rb->transform * translate(mat4(1),rb->velocity*dt);

        //Reset values
        rb->moments = {};
        //rb->angularVelocity = 0;

    }
}
