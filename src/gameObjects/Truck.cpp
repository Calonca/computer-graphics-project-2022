#include "Truck.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"



void Truck::UpdatePos(GLFWwindow* window, float deltaT)
{
    rb.angularVelocity.y = 0;
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
        rb.addLocalMoment(vec3( 10*ROT_SPEED * glm::vec4(0, 0, 1, 1) ),
                     vec3(-1,0,0));
        rb.addLocalMoment(vec3( 10*ROT_SPEED * glm::vec4(0, 0, -1, 1) ),
                     vec3(1,0,0));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
        rb.addLocalMoment(vec3( 10*ROT_SPEED * glm::vec4(0, 0, -1, 1) ),
                     vec3(-1,0,0));
        rb.addLocalMoment(vec3( 10*ROT_SPEED * glm::vec4(0, 0, 1, 1) ),
                     vec3(1,0,0));
	}
    const float LOOK_SPEED = 1.2f;
	if (glfwGetKey(window, GLFW_KEY_UP)) {
        camDelta = camDelta * rotate(mat4(1), LOOK_SPEED*deltaT,vec3(1,0,0));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        camDelta = camDelta * rotate(mat4(1),-LOOK_SPEED*deltaT,vec3(1,0,0));
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
        rb.addLocalMoment(vec3(ROT_SPEED * glm::vec4(0, 0, 1, 1)),
                          vec3(-1, 0, 0));
        rb.addLocalMoment(vec3( ROT_SPEED * glm::vec4(0, 0, -1, 1) ),
                     vec3(1,0,0));
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
        rb.addLocalMoment(vec3( ROT_SPEED * glm::vec4(0, 0, -1, 1) ),
                     vec3(-1,0,0));
        rb.addLocalMoment(vec3( ROT_SPEED * glm::vec4(0, 0, 1, 1) ),
                     vec3(1,0,0));
	}

	if (glfwGetKey(window, GLFW_KEY_W)) {
        rb.addLocalMoment(vec3(MOVE_SPEED * glm::vec4(0, 0, -1, 1)),
                          vec3(0, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
        rb.addLocalMoment(vec3(MOVE_SPEED * glm::vec4(0, 0, 1, 1)),
                          vec3(0, 0, 0));
	}


    //// Keys to debug
    static double old_xpos = 0, old_ypos = 0;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    double m_dx = xpos - old_xpos;
    double m_dy = ypos - old_ypos;
    old_xpos = xpos; old_ypos = ypos;
    //std::cout << xpos << " " << ypos << " " << m_dx << " " << m_dy << "\n";

    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        //lookPitch += m_dy * ROT_SPEED / MOUSE_RES;
        //lookYaw += m_dx * ROT_SPEED / MOUSE_RES;
    }

    const float CAM_UP_SPEED = 1.0f;
    if (glfwGetKey(window, GLFW_KEY_E)) {
        camDelta = translate(mat4(1),vec3(0,CAM_UP_SPEED*deltaT,0.9f*CAM_UP_SPEED*deltaT))*
                camDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        camDelta = translate(mat4(1),vec3(0,-CAM_UP_SPEED*deltaT,-0.9f*CAM_UP_SPEED*deltaT))*
                camDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_R)) {
        camDelta = translate(mat4(1),vec3(0,0,-10*CAM_UP_SPEED*deltaT))*
                   camDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_F)) {
        camDelta = translate(mat4(1),vec3(0,0,10*CAM_UP_SPEED*deltaT))*
                   camDelta;
    }
    ////

}
