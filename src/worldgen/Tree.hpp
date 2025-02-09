#include "glm/glm.hpp"
#include "../utils/definitions.hpp"
#include <vector>
#include <GLFW/glfw3.h>
#include "glm/gtc/matrix_transform.hpp"
#include "../gameObjects/Object.hpp"

extern struct Model g_test;

#pragma once
class Tree : public Collider,public Object
{
private:

public:
    Tree(mat4 transform);

    Tree(mat4 transform,bool t);

    void testCollision(CollisionObject *co);

};
