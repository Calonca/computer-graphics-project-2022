#pragma once

#include <utility>
#include <vector>
#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

enum PipelineType { Flat, Terrain };
struct RigidBody;

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
private:
    std::vector<vec3> points;//For now contains only one point
    mat4 t;
public:
    vec3 forceAfterCollision;
    bool isColliding =false;//default
    vec3 normal;
    CollisionObject(std::vector<vec3> points, const mat4 &t, const vec3 &forceAfterCollision, bool isColliding);

    vec3 getGlobalPoint (unsigned int i){
        if (i<0 || i>=points.size())
        {
            std::cout<<"Collision points do not contain the following index: "
            <<i<<"\nThe maximum allowed value is :"<<points.size()-1<<std::endl;
        }
        return t*vec4(points[i],1);
    }

    vec3 getLocalPoint (unsigned int i){
        if (i<0 || i>=points.size())
        {
            std::cout<<"Collision points do not contain the following index: "
                     <<i<<"\nThe maximum allowed value is :"<<points.size()-1<<std::endl;
        }
        return points[i];
    }


    int pointsLen (){
        return points.size();
    }
    void setTransform(RigidBody* r);
};

struct Moment {
    vec3 force;
    vec3 point;
    bool isGlobal;
};

/// <summary>
/// Component used ot hold physics values for entities
/// </summary>
struct RigidBody {
    mat4 transform;
    //float lookYaw;
    //float lookPitch;
    //float lookRoll;
	std::vector<Moment> moments;
	vec3 velocity;
	float mass;//Mass in kg

	vec3 fGravity;//Gravitational force in multiples of g
	bool hasGravity;//True if gravitational force is applied

	float staticFriction;
	float dynamicFriction;
	float bounciness;

    std::vector<CollisionObject> co;
    vec3 angularVelocity;

    //Adds a force to the object at the application point in the Rigidbody reference system.
    //The application point is a point in the local reference system
    void addLocalMoment(vec3 force, vec3 applyPoint){
        moments.push_back({force, applyPoint, false});
    }

    //Adds a force to the object at the application point in the global reference system.
    //Useful for forces like gravity that point always in the same direction regardless of the Rigidbody orientation
    //The application point is a point in the local reference system
    void addGlobalMoment(vec3 force, vec3 applyPoint){
        moments.push_back({force, applyPoint,true});
    }
};



//A volume that when point are in it generates collisions
struct Collider {
    //Function tha when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co) {
        co->isColliding= false;
    }
};


struct PlaneCollider : Collider {
    float planeY{};
    //Function that when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co) {
        float pointY = co->getGlobalPoint(0).y;
        //float planeHeight = planeY+2*po
        if (pointY < planeY){
            co->isColliding= true;
            co->forceAfterCollision=vec3(0,planeY - pointY,0);
            //std::cout<<"Is colliding: "<<co->forceAfterCollision.y;
        }else{
            co->isColliding= false;
            //std::cout<<"Not colliding";
        }
    }
};