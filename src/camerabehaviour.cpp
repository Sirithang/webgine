#include "CameraBehaviour.h"
#include "transform.h"
#include "vector3.h"
#include "input.h"


OrbitCamera::OrbitCamera()
{
	currentAngle = 0;
};

void OrbitCamera::update(float deltaTime)
{
	currentAngle += deltaTime * 25.0f;

	if(currentAngle > 360.0f)
		currentAngle = currentAngle - 360.0f;

	Entity& ent = getEntity(this->_entity);
	Transform& t = getTransform(ent._transform);

	alfar::Vector3 pos = transform::getWorldPosition(ent._transform);

	const float speed = 40.0f;

	if(input::isKeyDown('E'))
	{
		pos = alfar::vector3::add(pos, alfar::vector3::mul(transform::getForward(ent._transform), deltaTime *speed));
	}
	else if(input::isKeyDown('D'))
	{
		pos = alfar::vector3::add(pos, alfar::vector3::mul(transform::getForward(ent._transform), -deltaTime * speed));
	}

	if(input::isKeyDown('S'))
	{
		pos = alfar::vector3::add(pos, alfar::vector3::mul(transform::getRight(ent._transform), -deltaTime * speed));
	}
	else if(input::isKeyDown('F'))
	{
		pos = alfar::vector3::add(pos, alfar::vector3::mul(transform::getRight(ent._transform), deltaTime * speed));
	}

	if(input::isKeyDown('R'))
	{
		pos = alfar::vector3::add(pos, alfar::vector3::mul(transform::getUp(ent._transform), -deltaTime * speed));
	}
	else if(input::isKeyDown('T'))
	{
		pos = alfar::vector3::add(pos, alfar::vector3::mul(transform::getUp(ent._transform), deltaTime * speed));
	}

	if(input::isKeyDown('K'))
	{
		transform::rotate(ent._transform, alfar::vector3::create(0,1,0), -2.0f * 3.14f / 180.0f);
	}
	else if(input::isKeyDown('L'))
	{
		transform::rotate(ent._transform, alfar::vector3::create(0,1,0), 2.0f * 3.14f / 180.0f);
	}


	transform::setPosition(ent._transform, pos);
	//transform::setAxisAngleRotation(ent._transform, alfar::vector3::create(0,0,0), 0/*-20 * 3.14f / 180.0f*/);

	//transform::rotate(ent._transform, alfar::vector3::create(0,1,0), currentAngle * 3.14f / 180.0f);
	//transform::rotate(ent._transform, alfar::vector3::create(1,0,0), -20 * 3.14f / 180.0f);
}