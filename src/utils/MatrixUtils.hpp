#ifndef CGPROJECT_MATRIXUTILS_HPP
#define CGPROJECT_MATRIXUTILS_HPP

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

    static void printVector(vec3 v) {
        std::cout<< "x: "+std::to_string(v.x)+", y: "+std::to_string(v.y)+", z: "+std::to_string(v.z)<<std::endl;
    };

    static mat4 LookAtMat(vec3 Pos, vec3 aim, vec3 up);

    static vec3 fromGlobalToLocal(mat4 transform, vec3 global);
};




#endif //CGPROJECT_MATRIXUTILS_HPP
