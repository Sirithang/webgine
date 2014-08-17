#pragma once

#include "math_types.h"
#include "collection_types.h"
#include "objectmanager.h"
#include "entity.h"

struct Transform : public ManagedObject
{
	alfar::Vector3 _position;
	alfar::Vector3 _scale;
	alfar::Quaternion _rotation;

	alfar::Matrix4x4 _matrix;

	alfar::Vector3 _forward;
	alfar::Vector3 _up;
	alfar::Vector3 _right;
};

DECLARE_MANAGED(Transform, 1024)

namespace transform
{
	TransformID create(EntityID owner);

	void init(Transform& transform);

	void setPosition(TransformID transform, alfar::Vector3 position);

	void updateMatrix(TransformID tranform);
}

