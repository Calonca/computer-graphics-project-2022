#ifndef CGPROJECT_MATRIXUTILS_H
#define CGPROJECT_MATRIXUTILS_H

#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace glm;

class MatrixUtils {
public:
    // Create a look at matrix
    // Pos    -> pos of the camera (c)
    // aim    -> Posizion of the target (a)
    // Roll   -> roll (rho)
    static mat4 LookAtMat(vec3 Pos, vec3 aim, float Roll);


    // Create a look in direction matrix
    // Pos    -> pos of the camera
    // Angs.x -> direction (alpha)
    // Angs.y -> elevation (beta)
    // Angs.z -> roll (rho)
    static mat4 LookInDirMat(vec3 Pos, vec3 Angs);

    static std::string printVector(vec3 v) {
        return "x: "+std::to_string(v.x)+", y: "+std::to_string(v.y)+", z: "+std::to_string(v.z);
    };

    static mat4 LookAtMat(vec3 Pos, vec3 aim, vec3 up);
};


#endif //CGPROJECT_MATRIXUTILS_H
