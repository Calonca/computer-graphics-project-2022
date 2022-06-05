//
// Created by calonca on 6/5/22.
//

#include <stdlib.h>
#include <map>
#include <vector>
#include "../utils/definitions.h"

#ifndef CGPROJECT_OBJECT_H
#define CGPROJECT_OBJECT_H


class Object {
private:
    mat4 transform;
    void addChildrenToVector(std::vector<Object>& res){
        for(auto child : children){
            if (child.model.scale >0) {
                res.push_back(child);
                child.addChildrenToVector(res);
            }
        }
    };
public:
    std::string id;
    Object* pParent;
    Model model = {"","",0,Flat};
    std::vector<Object> children;

    Object(std::string id, Model model, mat4 transform);

    Object(std::string id, mat4 transform);


    mat4 getTransform() const;
    Object * addObject(std::string id, Model model, mat4 t);

    void addObject(Object o);


    std::vector<Object> getAllChildrenWithModels(){
        std::vector<Object> result;
        addChildrenToVector(result);
        return result;
    };

    int countChildrenWithModels(){
        int result = 0;
        result = children.size();
        for(auto child : children){
            if (child.model.scale >0) {
                result += child.countChildrenWithModels();
            }
        }
        return result;
    };


};


#endif //CGPROJECT_OBJECT_H
