//
// Created by calonca on 6/5/22.
//

#include "Object.hpp"

#include <utility>

mat4 Object::getGlobalTransform() const {
    return globalTransform;
}

mat4 getTransform(Object* o) {
    if (o->pParent!= nullptr)
        return getTransform(o->pParent)*o->getLocalTransform();
    else
        return o->getLocalTransform();
}

void Object::setRecursiveGlobalTransform() {
    if (pParent!= nullptr)
        globalTransform = pParent->globalTransform*localTransform;
    else globalTransform = localTransform;

    for (Object *child : children) {
        child->setRecursiveGlobalTransform();
    }

}

mat4 Object::getLocalTransform() const {
    return localTransform;
}

Object::Object(std::string id, const Model model, const mat4 localTransform) : localTransform(localTransform), id(std::move(id)),
                                                                                   model(model) {
    globalTransform = localTransform;
}




Object::Object(std::string id, const mat4 localTransform) : localTransform(localTransform), id(std::move(id)) {
    globalTransform = localTransform;
}


Object* Object::addObject(std::string identifier, Model m, mat4 t) {
    auto* o = new Object(std::move(identifier), m, t);
    o->pParent= this;
    children.push_back(o);
    if (o->model.scale>0)
        objs.push_back(o);
    o->setRecursiveGlobalTransform();
    return o;
}




void Object::addObject(Object& o) {
    o.pParent= this;
    children.push_back(&o);
    if (o.model.scale>0)
        objs.push_back(&o);
    o.setRecursiveGlobalTransform();
}
std::vector<Object*>  Object::objs = {} ;

void Object::setTransform(const mat4 &t) {
    localTransform = t;
    setRecursiveGlobalTransform();
}
