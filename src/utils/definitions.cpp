#include "definitions.hpp"

void CollisionObject::setTransform(RigidBody *r) {
    t = r->parent->getTransform();
}

CollisionObject::CollisionObject(
        std::vector<vec3> points,
        const mat4 &t,
        const vec3 &forceAfterCollision,
        bool isColliding)
        :
        points(std::move(points)),
        t(t),
        forceAfterCollision(forceAfterCollision),
        isColliding(isColliding) {}
