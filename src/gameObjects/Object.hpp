//
// Created by calonca on 6/5/22.
//
#pragma once
#ifndef CGPROJECT_OBJECT_H
#define CGPROJECT_OBJECT_H

#include <stdlib.h>
#include <map>
#include <vector>
#include <string>
#include "glm/glm.hpp"
#include "../utils/Model.hpp"

using namespace glm;

class Object {
private:
    mat4 localTransform;
    mat4 globalTransform;
    virtual void setRecursiveGlobalTransform();

    /*
    void addChildrenToVector(std::vector<Object>& res){
        for(Object* child : children){
            if (child->model.scale >0) {
                res.push_back(*child);
                //std::cout<<"Adding child: "<< child->id<<std::endl;
                child->addChildrenToVector(res);
            }
        }
    };*/
public:
    static std::vector<Object*> objs;//Contains list of all the objects with children
    std::string id;
    Object* pParent;
    Model model = {"","",0,Flat};
    std::vector<Object*> children;//Std vector makes a copy of the Objects

    Object(std::string id, Model model, mat4 localTransform);

    Object(std::string id, mat4 localTransform);

    virtual mat4 getGlobalTransform() const;
    virtual mat4 getLocalTransform() const;

    void setTransform(const mat4 &t);

    Object * addObject(std::string id, Model model, mat4 t);
    void addObject(Object& o);


    /*
    std::vector<Object> getAllChildrenWithModels(){
        std::vector<Object> result;
        addChildrenToVector(result);
        return result;
    };

    int countChildrenWithModels(){
        int result = 0;
        result = children.size();
        for(auto child : children){
            if (child->model.scale >0) {
                result += child->countChildrenWithModels();
            }
        }
        return result;
    };
    */

};


#endif //CGPROJECT_OBJECT_H
