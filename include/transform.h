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

	ID _parent;
};

DECLARE_MANAGED(Transform, 1024)

namespace transform
{
	TransformID create(EntityID owner);

	void init(Transform& transform);

	void setParent(TransformID target, TransformID parent);

	void setPosition(TransformID transform, alfar::Vector3 position);
	void setAxisAngleRotation(TransformID transform, alfar::Vector3 axis, float angle);

	//Add that rotation to the previous one
	void rotate(TransformID transform, alfar::Vector3 axis, float angle);

	alfar::Vector3 getUp(TransformID target);
	alfar::Vector3 getRight(TransformID target);
	alfar::Vector3 getForward(TransformID target);
	alfar::Vector3 getWorldPosition(TransformID target);

	void update();

	void updateMatrix(TransformID tranform);
}

