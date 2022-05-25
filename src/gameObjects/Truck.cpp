#include "Truck.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"


void Truck::UpdatePos(GLFWwindow* window, float deltaT)
{
	rb.force = vec3(0, 0, 0);
	if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		rb.rot *= quat(vec3(0, ROT_SPEED * deltaT, 0));
		lookYaw += deltaT * ROT_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		rb.rot *= quat(vec3(0, -ROT_SPEED * deltaT, 0));
		lookYaw -= deltaT * ROT_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		rb.rot *= quat(vec3(ROT_SPEED * deltaT,0, 0));
		lookPitch += deltaT * ROT_SPEED;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		rb.rot *= quat(vec3(-ROT_SPEED * deltaT,0, 0));
		lookPitch -= deltaT * ROT_SPEED;
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
		rb.rot = quat(vec3(0,ROT_SPEED*deltaT,0)) * rb.rot;
		lookYaw += deltaT * ROT_SPEED;
		//rb.force += vec3(MOVE_SPEED * mat4(quat(glm::vec3(0, lookYaw, 0))) * glm::vec4(-1, 0, 0, 1) * deltaT);
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		rb.rot = quat(vec3(0, -ROT_SPEED * deltaT, 0)) * rb.rot;
		lookYaw -= deltaT * ROT_SPEED;
		//rb.force += vec3(-2*MOVE_SPEED * mat4(quat(glm::vec3(0, lookYaw, 0))) * glm::vec4(-1, 0, 0, 1) * deltaT);
	}

	rb.rot = glm::quat(glm::vec3(0, lookYaw, 0)) *
		glm::quat(glm::vec3(lookPitch, 0, 0)) *
		glm::quat(glm::vec3(0, 0, lookRoll));

	if (glfwGetKey(window, GLFW_KEY_W)) {
		rb.force += vec3( MOVE_SPEED * mat4(quat(glm::vec3(0, lookYaw, 0))) * glm::vec4(0, 0, -1, 1) * deltaT);
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		rb.force += vec3(- MOVE_SPEED * mat4(quat(glm::vec3(0, lookYaw, 0))) * glm::vec4(0, 0, -1, 1) * deltaT);
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
        lookPitch += m_dy * ROT_SPEED / MOUSE_RES;
        lookYaw += m_dx * ROT_SPEED / MOUSE_RES;
    }

    if (glfwGetKey(window, GLFW_KEY_E)) {
        RobotCamDeltaPos.y += 0.01;
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        RobotCamDeltaPos.y -= 0.01;
    }
    ////

	vec3 eul = eulerAngles(rb.rot);

}
