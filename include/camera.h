#pragma once

#include "objectmanager.h"
#include "math_types.h"
#include "entity.h"

struct Camera : public ManagedObject
{
	alfar::Matrix4x4 view;
	alfar::Matrix4x4 projection;

	ID _tn;
};

DECLARE_MANAGED(Camera, 256)

namespace camera
{
	CameraID create(EntityID owner);
	void reset(Camera& cam);

	void update();
	void updateMatrices(Camera& cam);
}