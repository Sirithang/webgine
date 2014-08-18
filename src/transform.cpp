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
}

//--------------------------------

void transform::setPosition(TransformID transform, alfar::Vector3 position)
{
	Transform& t = getTransform(transform);

	t._position = position;

	updateMatrix(transform);
}

//--------------------------------

//--------------------------------

void transform::updateMatrix(TransformID tranform)
{
	Transform& t = getTransform(tranform);

	t._matrix = alfar::mat4x4::identity();
	t._matrix = alfar::mat4x4::mul(t._matrix, alfar::mat4x4::translation(t._position));
	t._matrix = alfar::mat4x4::mul(t._matrix, alfar::quaternion::toMat4x4(t._rotation));

	t._forward = alfar::vector4::toVec3(alfar::vector4::mul(t._matrix, alfar::vector4::create(0,0,1,0)));
	t._up = alfar::vector4::toVec3(alfar::vector4::mul(t._matrix, alfar::vector4::create(0,1,0,0)));
	t._right = alfar::vector4::toVec3(alfar::vector4::mul(t._matrix, alfar::vector4::create(1,0,0,0)));
}