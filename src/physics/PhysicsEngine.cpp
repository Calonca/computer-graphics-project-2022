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
    SolveCollisions(dt);
    ApplyForces(dt);
}



//Adds forces to rigidbodies based on collisions
void PhysicsEngine::SolveCollisions(float dt) {
    //For each collider check collision objects and set force
    for(TerrainCollider* collider : colliders){
        for (RigidBody* rb : rbs) {
            if (!rb->co) continue;
            CollisionObject* collisionObject =rb->co;
            collisionObject->setTransform(rb);
            //PlaneCollider* pl = dynamic_cast<PlaneCollider*>(collider);
            collider->testCollision(collisionObject);
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
                //std::cout<<"Velocity delta after impulse: "<< MatrixUtils::printVector(-impluse * aInvMass)<< std::endl;

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
                rb->force +=  collisionObject->forceAfterCollision*rb->mass*1000.0f*dt;

                //std::cout<<"Impulse force: "<< MatrixUtils::printVector(collisionObject->forceAfterCollision*50000.0f*dt)<< std::endl;
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
        //std::cout<<"Resulting force: "<< MatrixUtils::printVector(rb->force)<< std::endl;
        rb->velocity += (rb->force / rb->mass) * dt;
        rb->velocity = rotate(mat4(1),rb->angularVelocity,vec3(0,1,0))*vec4(rb->velocity,1);

        rb->transform = rotate(rb->transform,rb->angularVelocity,vec3(0,1,0));

        rb->transform =
                translate(mat4(1),rb->velocity*dt)
                * rb->transform;
                 // Translation in global coordinates system

        //Reset values
        rb->force = vec3(0, 0, 0);
        rb->angularVelocity = 0;

    }
}
/*
stbi_uc* stationMap;
int stationMapWidth, stationMapHeight;
bool canStepPoint(float x, float y) {
    int pixX = round(fmax(0.0f, fmin(stationMapWidth-1,  (x+10) * stationMapWidth  / 20.0)));
    int pixY = round(fmax(0.0f, fmin(stationMapHeight-1, (y+10) * stationMapHeight / 20.0)));
    int pix = (int)stationMap[stationMapWidth * pixY + pixX];
//std::cout << pixX << " " << pixY << " " << x << " " << y << " \t P = " << pix << "\n";
    return pix > 128;
}
const float checkRadius = 0.1;
const int checkSteps = 12;
bool canStep(float x, float y) {
    for(int i = 0; i < checkSteps; i++) {
        if(!canStepPoint(x + cos(6.2832 * i / (float)checkSteps) * checkRadius,
                         y + sin(6.2832 * i / (float)checkSteps) * checkRadius)) {
            return false;
        }
    }
    return true;
}

 		stationMap = stbi_load((TEXTURE_PATH + "MapSciFiStep.png").c_str(),
							&stationMapWidth, &stationMapHeight,
							NULL, 1);
		if (!stationMap) {
			std::cout << (TEXTURE_PATH + "MapSciFiStep.png").c_str() << "\n";
			throw std::runtime_error("failed to load map image!");
		}
		std::cout << "Station map -> size: " << stationMapWidth
				  << "x" << stationMapHeight <<"\n";
*/