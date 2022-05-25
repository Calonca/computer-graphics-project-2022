#ifndef CGPROJECT_MATRIXUTILS_H
#define CGPROJECT_MATRIXUTILS_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

using namespace glm;

class MatrixUtils {
public:
    // Create a look at matrix
    // Pos    -> Position of the camera (c)
    // aim    -> Posizion of the target (a)
    // Roll   -> roll (rho)
    static mat4 LookAtMat(vec3 Pos, vec3 aim, float Roll);


    // Create a look in direction matrix
    // Pos    -> Position of the camera
    // Angs.x -> direction (alpha)
    // Angs.y -> elevation (beta)
    // Angs.z -> roll (rho)
    static mat4 LookInDirMat(vec3 Pos, vec3 Angs);
};


#endif //CGPROJECT_MATRIXUTILS_H
