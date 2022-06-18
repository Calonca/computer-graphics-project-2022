#include "Truck.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/quaternion.hpp"


bool rotating = false;
float wheelAngle = 0.0f;
void Truck::UpdatePos(GLFWwindow* window, float deltaT)
{
    rb.angularVelocity.y -= 0.06f*rb.angularVelocity.y;
    rotating = false;

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
        firstPersonCamDelta = firstPersonCamDelta * rotate(mat4(1), LOOK_SPEED * deltaT, vec3(1, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN)) {
        firstPersonCamDelta = firstPersonCamDelta * rotate(mat4(1), -LOOK_SPEED * deltaT, vec3(1, 0, 0));
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
        rotating = true;
        rb.addLocalMoment(vec3(ROT_SPEED * glm::vec4(0, 0, 1, 1)),
                          vec3(-1, 0, 0));
        rb.addLocalMoment(vec3( ROT_SPEED * glm::vec4(0, 0, -1, 1) ),
                     vec3(1,0,0));

        wheelAngle = rotateWheels(wheelAngle, -30.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
        rotating = true;
        rb.addLocalMoment(vec3( ROT_SPEED * glm::vec4(0, 0, -1, 1) ),
                     vec3(-1,0,0));
        rb.addLocalMoment(vec3( ROT_SPEED * glm::vec4(0, 0, 1, 1) ),
                     vec3(1,0,0));
        wheelAngle = rotateWheels(wheelAngle, 30.0f);
    }

    if (!rotating) {
        wheelAngle = rotateWheels(wheelAngle, 0.0f);
    }

	if (glfwGetKey(window, GLFW_KEY_W)) {
        rb.addLocalMoment(vec3(MOVE_SPEED*deltaT * glm::vec4(0, 0, -1, 1)),
                          vec3(0, 0, 0));
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
        rb.addLocalMoment(vec3(MOVE_SPEED*deltaT * glm::vec4(0, 0, 1, 1)),
                          vec3(0, 0, 0));
	}

    for (int i = 2; i < 6; i++) {
        Object* wheel = children[i];
        //Rotate wheel by angular velocity
        float radius = 0.2;
        float angle = rb.velocity.z*radius*0.1f;
        if(wheel->id=="wheelFR" || wheel->id=="wheelBR")//Right wheels
            angle = -angle;
        wheel->setTransform(wheel->getLocalTransform()*rotate(mat4(1), angle, vec3(1, 0, 0)));
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

    const float CAM_UP_SPEED = 10.0f;
    if (glfwGetKey(window, GLFW_KEY_E)) {
        firstPersonCamDelta = translate(mat4(1), vec3(0, CAM_UP_SPEED * deltaT, 0.9f * CAM_UP_SPEED * deltaT)) *
                              firstPersonCamDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_Q)) {
        firstPersonCamDelta = translate(mat4(1), vec3(0, -CAM_UP_SPEED * deltaT, -0.9f * CAM_UP_SPEED * deltaT)) *
                              firstPersonCamDelta;
    }

    if (glfwGetKey(window, GLFW_KEY_R)) {
        firstPersonCamDelta = translate(mat4(1), vec3(0, 0, -10 * CAM_UP_SPEED * deltaT)) *
                              firstPersonCamDelta;
    }
    if (glfwGetKey(window, GLFW_KEY_F)) {
        firstPersonCamDelta = translate(mat4(1), vec3(0, 0, 10 * CAM_UP_SPEED * deltaT)) *
                              firstPersonCamDelta;
    }
    ////

}

Model m = {"MonsterTruck/Truck.obj", "MonsterTruck/Truck.png", 1, Flat };
Model wheel = {"MonsterTruck/wheel.obj", "MonsterTruck/Truck.png", 1, Flat };
//Truck::Truck(const std::string &id, const Model &model, const mat4 &transform) : Object(id, model, transform) {}
Truck::Truck() : Object("truck", m, initialTransform) {
    rb.parent = this;
    auto* leftLight = new Object("leftLight",translate(mat4(1),vec3(-0.4,1,-0.5)));
    auto* rightLight = new Object("rightLight",translate(mat4(1),vec3(0.4,1,-0.5)));
    addObject(*leftLight);
    addObject(*rightLight);

    float height = 0.4f;
    float sideDist = 0.5f;
    float frontDist = 0.7f;
    float backDist = 0.65f;

    mat4 rotate180Y = rotate(mat4(1), radians(180.0f), vec3(0, 1, 0));

    addObject("wheelFL",wheel, translate(mat4(1),vec3(-sideDist, height, -frontDist)) );
    addObject("wheelFR",wheel, translate(mat4(1),vec3(sideDist, height, -frontDist)) * rotate180Y );

    addObject("wheelBL",wheel, translate(mat4(1),vec3(-sideDist, height, backDist)) );
    addObject("wheelBR",wheel, translate(mat4(1),vec3(sideDist, height, backDist)) * rotate180Y );
}
