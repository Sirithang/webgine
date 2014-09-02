#include "transform.h"

#include "vector3.h"
#include "quaternion.h"
#include "mat4x4.h"
#include "enums.h"

IMPLEMENT_MANAGED(Transform)

TransformID transform::create(EntityID owner)
{
	TransformID id = gTransformManager.add();
	Transform& t = getTransform(id);

	init(t);
	updateMatrix(id);

	entity::addComponent(owner, id, TRANSFORM);

	return id;
}

//----------------------------------

void transform::init(Transform& transform)
{
	transform._position = alfar::vector3::create(0,0,0);
	transform._scale = alfar::vector3::create(1,1,1);
	transform._rotation = alfar::quaternion::identity();

	transform._parent = -1;
}

//--------------------------------

void transform::setPosition(TransformID transform, alfar::Vector3 position)
{
	Transform& t = getTransform(transform);

	t._position = position;

	updateMatrix(transform);
}

//--------------------------------

void transform::setAxisAngleRotation(TransformID transform, alfar::Vector3 axis, float angle)
{
	Transform& t = getTransform(transform);

	t._rotation = alfar::quaternion::axisAngle(axis, angle);

	updateMatrix(transform);
}

//--------------------------------

void transform::rotate(TransformID transform, alfar::Vector3 axis, float angle)
{
	Transform& t = getTransform(transform);

	t._rotation = alfar::quaternion::mul(alfar::quaternion::axisAngle(axis, angle), t._rotation);

	updateMatrix(transform);
}

//--------------------------------

void transform::update()
{
	for(int i = 0; i < gTransformManager._num_objects; ++i)
	{
		Transform& t = gTransformManager._objects[i];

		updateMatrix(t.id);
	}
}

//--------------------------------

void transform::setParent(TransformID target, TransformID parent)
{
	Transform& t  = getTransform(target);
	t._parent = parent;
}

//--------------------------------

void transform::updateMatrix(TransformID tranform)
{
	Transform& t = getTransform(tranform);

	alfar::Matrix4x4 parentMatrix = alfar::mat4x4::identity();
	if(t._parent != -1)
	{
		updateMatrix(t._parent);
		parentMatrix = getTransform(t._parent)._matrix;
	}


	t._matrix = alfar::mat4x4::identity();
	//t._matrix = alfar::mat4x4::mul(parentMatrix, t._matrix);
	t._matrix = alfar::mat4x4::mul(t._matrix, alfar::mat4x4::translation(t._position));
	t._matrix = alfar::mat4x4::mul(t._matrix, alfar::quaternion::toMat4x4(t._rotation));

	t._matrix = alfar::mat4x4::mul(parentMatrix, t._matrix);

	t._forward = alfar::vector4::toVec3(alfar::vector4::mul(t._matrix, alfar::vector4::create(0,0,1,0)));
	t._up = alfar::vector4::toVec3(alfar::vector4::mul(t._matrix, alfar::vector4::create(0,1,0,0)));
	t._right = alfar::vector4::toVec3(alfar::vector4::mul(t._matrix, alfar::vector4::create(1,0,0,0)));
}

//-------------------------------------

alfar::Vector3 transform::getUp(TransformID target)
{
	Transform& t = getTransform(target);

	return t._up;
}

//-------------------------------------

alfar::Vector3 transform::getRight(TransformID target)
{
	Transform& t = getTransform(target);

	return t._right;
}

//-------------------------------------

alfar::Vector3 transform::getForward(TransformID target)
{
	Transform& t = getTransform(target);

	return t._forward;
}

//-------------------------------------

alfar::Vector3 transform::getWorldPosition(TransformID target)
{
	Transform& t = getTransform(target);

	alfar::Vector4 v = alfar::vector4::mul(t._matrix, alfar::vector4::create(0.0f,0.0f,0.0f, 1.0f));

	return alfar::vector3::create(v.x/v.w, v.y/v.w, v.z/v.w);
}