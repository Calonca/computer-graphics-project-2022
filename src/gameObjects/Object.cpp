//
// Created by calonca on 6/5/22.
//

#include "Object.h"

#include <utility>

mat4 Object::getTransform() {
    if (pParent!= nullptr)
        return pParent->getTransform()*transform;
    else
        return transform;
}

Object::Object(std::string id, const Model model, const mat4 transform) : transform(transform), id(std::move(id)),
                                                                                   model(model) {}
Object::Object(std::string id, const mat4 transform) : transform(transform), id(std::move(id)) {}


void Object::addObject(std::string identifier, Model m, mat4 t) {
    Object o = Object(std::move(identifier), m, t);
    SceneModel sm = {};
    o.model.pSceneModel = &sm;
    o.pParent= this;
    children.push_back(o);
}
