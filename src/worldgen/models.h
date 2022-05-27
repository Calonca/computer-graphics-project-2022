//
// Created by calonca on 5/25/22.
//

#ifndef CGPROJECT_MODELS_H
#define CGPROJECT_MODELS_H

#include "../utils/definitions.h"
#include "../utils/MatrixUtils.h"

#pragma once
class models
{
public:
    //void makeModels(std::vector<float> M1_vertices, std::vector<uint32_t> M1_indices);
    static std::vector<vec3> tile_pos(float x ,float y,float z);
    // static vec3 normal_triangletile(float x, float y, float z);

};

struct TerrainCollider : Collider {
    //Function that when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co) {

        vec3 firstPoint = co->getPoint(0);
        //std::cout<<"First point position: "<< MatrixUtils::printVector(firstPoint)<< std::endl;
        std::vector<vec3> triang = models::tile_pos(firstPoint.x,firstPoint.y,firstPoint.z);
        //models::normal_triangletile(firstPoint.x, firstPoint.y, firstPoint.z);
        float force = 0;
        co->forceAfterCollision = { 0,0,0 };
        co->isColliding = false;

        if (firstPoint.y < triang[3][1]) {
            co->isColliding = true;
            force = abs(firstPoint.y - triang[3][1]);
            //std::cout << "val yyy  " << triang[3][1];

            //std::cout << "force" << force<<std::endl;

            co->forceAfterCollision = { 0,force,0 };
        }

    }
};



#endif //CGPROJECT_MODELS_H
