#include "../utils/definitions.h"
#include <stdio.h>
#include <vector>
#include "../utils/MatrixUtils.h"
#include <iostream>
#include "../worldgen/models.h"

#pragma once
class PhysicsEngine
{
private:
	std::vector<RigidBody*> rbs;
    std::vector<TerrainCollider*> colliders;

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
    void SolveCollisions(float dt);

    void AddCollider(TerrainCollider* collider);
    void RemoveCollider(TerrainCollider* c);

	/// <summary>
	/// Applies the forces and calculates the values of physical quantities after the dt in
	/// </summary>
	/// <param name="dt">physics step time</param>
	void Step(float dt);

    void ApplyForces(float dt);
};

