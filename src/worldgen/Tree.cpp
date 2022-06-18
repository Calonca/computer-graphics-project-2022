#include "Tree.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"

Model treeM = { "tree.obj", "Colors.png", 1, Flat };
//Truck::Truck(const std::string &id, const Model &model, const mat4 &transform) : Object(id, model, transform) {}
Tree::Tree(mat4 transform) : Object("tree", treeM, transform) {

}

void Tree::testCollision(CollisionObject *co) {
    vec3 colliderPos = getGlobalTransform()[3];
    vec3 objectPos = co->getGlobalPosition();

    //If truck is far away, don't check for collision
    if (abs(objectPos.x - colliderPos.x) > 5.0f ||
        abs(objectPos.z - colliderPos.z) > 5.0f ||
        abs(objectPos.y - colliderPos.y) > 5.0f
            ) {
        co->isColliding = false;
        co->forceAfterCollision = vec3(0, 0, 0);
        //std::cout<<"Collision is false"<<std::endl;
        return;
    }

    vec3 force = vec3(0, 0, 0);
    bool isColliding = false;
    int nPoints = co->pointsLen();
    for (int i = 0; i < nPoints; i++) {
        vec3 point = co->getGlobalPoint(i);
        point.y = 0;
        colliderPos.y = 0;
        vec3 diff = point - colliderPos;
        co->normal = normalize(diff);
        float dist = length(diff);
        if (dist < 0.4) {
            isColliding = true;
            force += co->normal*(1/dist);
        }
    }
    co->isColliding = isColliding;
    co->forceAfterCollision = force;
}
