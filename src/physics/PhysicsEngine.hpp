#include "../utils/definitions.hpp"
#include <stdio.h>
#include <vector>
#include "../utils/MatrixUtils.hpp"
#include <iostream>
#include "../worldgen/Terrain.hpp"


#include <functional>
#include <thread>
#include <chrono>
#include <GLFW/glfw3.h>

#pragma once
class PhysicsEngine
{
private:
	std::vector<RigidBody*> rbs;
    std::vector<Collider*> colliders;

public:
	/// <summary>
	/// Adds rigid body to the physics engine so that physics can be calculated
	/// </summary>
	/// <param name="rb"></param>
	void AddRigidBody(RigidBody* rb);
	/// <summary>
	/// Removes rigid bodies from the physics engine 
	/// </summary>
	/// <param name="rb"></param>
	void RemoveRigidBody(RigidBody* rb);

    void ApplyGravity();
    void SolveCollisions(GLFWwindow *window);

    void AddCollider(Collider* collider);
    void RemoveCollider(Collider* c);

	/// <summary>
	/// Applies the forces and calculates the values of physical quantities after the dt in
	/// </summary>
	/// <param name="dt">physics step time</param>
    void Step(float dt, GLFWwindow *window);

    //Used to test step on another thread
    void StepC(PhysicsEngine* p){

        while (true){
            static auto startTime = std::chrono::high_resolution_clock::now();
            static float lastTime = 0.0f;

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>
                    (currentTime - startTime).count();
            float deltaT = time - lastTime;
            lastTime = time;

            const float TIMESTEP = 0.003f;//in seconds
            float timeDiff = TIMESTEP-deltaT;
            p->Step(timeDiff, nullptr);
            if (timeDiff<0)
                std::cout<<"Physics is too slow, increase the timestep by "<<timeDiff<<" seconds"<<std::endl;
            std::this_thread::sleep_for (std::chrono::milliseconds (int(1000*timeDiff)));
        }
    };

    void ApplyForces(float dt);
};

