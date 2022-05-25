//
// Created by calonca on 5/25/22.
//

#ifndef CGPROJECT_MODELS_H
#define CGPROJECT_MODELS_H

#include "../utils/definitions.h"

#pragma once
class models
{
public:
    static std::vector<vec3> tile_pos(float x ,float y,float z);
};

struct TerrainCollider : Collider {
    //Function that when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co, vec3 translation) {

        std::vector<vec3> triang = models::tile_pos(co->points[0].x+translation.x, co->points[0].y+translation.y, co->points[0].z+translation.z);
        float force = 0;
        co->forceAfterCollision = { 0,0,0 };

        if (co->points[0][1]+translation.y < triang[3][1]) {
            co->isColliding = true;
            force = abs(co->points[0][1]+translation.y - triang[3][1]);
            //std::cout << "val yyy  " << triang[3][1];

            //std::cout << "force" << force<<std::endl;

            co->forceAfterCollision = { 0,force,0 };
        }

    }
};

#endif //CGPROJECT_MODELS_H
