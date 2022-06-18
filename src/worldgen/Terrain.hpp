

#pragma once
#ifndef CGPROJECT_TERRAIN_HPP
#define CGPROJECT_TERRAIN_HPP

#include "../utils/definitions.hpp"
#include "../utils/MatrixUtils.hpp"
#include "Noise.hpp"

struct VertAndIndices{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
};

class Terrain : public Collider
{
public:
    static const int TILE_NUMBER = 300;
    static VertAndIndices makeModels();
    static float getHeight(float xoff, float zoff);
    //void makeModels(std::vector<float> M1_vertices, std::vector<uint32_t> M1_indices);
    static std::vector<vec3> tile_pos(float x ,float y,float z);
    // static vec3 normal_triangletile(float x, float y, float z);
    static vec3 normalTriangleTile(float x, float y, float z);

    static float interpolate_y(float x, float z, float close1, float close2, float* t1, float* t2);
    //static vec3 interpolate(float close1, float close2,);

    //Function that when given an object containing points
    // returns a force
    void testCollision(CollisionObject *co);
};

#endif //CGPROJECT_TERRAIN_HPP
