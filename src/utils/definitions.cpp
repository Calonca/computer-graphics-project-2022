#include "definitions.h"

void CollisionObject::setTransform(RigidBody *r) {
    t.pos = r->pos;
}

CollisionObject::CollisionObject(
        std::vector<vec3> points,
        const Transform &t,
        const vec3 &forceAfterCollision,
        bool isColliding)
        :
        points(std::move(points)),
        t(t),
        forceAfterCollision(forceAfterCollision),
        isColliding(isColliding) {}
