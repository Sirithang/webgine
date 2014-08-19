#include "camera.h"
#include "mat4x4.h"
#include "quaternion.h"
#include "transform.h"

#include <stdio.h>

IMPLEMENT_MANAGED(Camera)

CameraID camera::create(EntityID owner)
{
	CameraID id = gCameraManager.add();
	Camera& c = getCamera(id);

	reset(c);

	c._tn = getEntity(owner)._transform;

	entity::addComponent(owner, id, CAMERA);

	return id;
}

void camera::reset(Camera& c)
{
	c.projection = alfar::mat4x4::persp(3.14f / 2.0f, 4.0f/3.0f, 0.1f, 100.0f);
	c.view = alfar::mat4x4::identity();
}

void camera::updateMatrices(Camera& cam)
{
	Transform& t = getTransform(cam._tn);
	cam.view = alfar::mat4x4::lookAt(t._position, alfar::vector3::add(t._position, t._forward), t._up );
}


//this will update all matrix
void camera::update()
{
	for(int i = 0; i < gCameraManager._num_objects; ++i)
	{
		camera::updateMatrices(gCameraManager._objects[i]);
	}
}