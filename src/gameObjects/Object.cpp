//
// Created by calonca on 6/5/22.
//

#include "Object.h"

#include <utility>

mat4 Object::getTransform() const {
    if (pParent!= nullptr)
        return pParent->getTransform()*transform;
    else
        return transform;
}

Object::Object(std::string id, const Model model, const mat4 transform) : transform(transform), id(std::move(id)),
                                                                                   model(model) {}
Object::Object(std::string id, const mat4 transform) : transform(transform), id(std::move(id)) {}


Object* Object::addObject(std::string identifier, Model m, mat4 t) {
    auto* o = new Object(std::move(identifier), m, t);
    o->pParent= this;
    children.push_back(o);
    return o;
}

void Object::addObject(Object& o) {
    o.pParent= this;
    children.push_back(&o);
}
