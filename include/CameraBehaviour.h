#pragma once 

#include "behaviour.h"
#include "math_types.h"

struct OrbitCamera : public BehaviourInterface
{
	float currentAngle;

	OrbitCamera();

	virtual void update(float deltaTime);
};