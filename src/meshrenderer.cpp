#include "meshrenderer.h"
#include "vector3.h"
#include <stdio.h>

IMPLEMENT_MANAGED(MeshRenderer);

MeshRendererID meshrenderer::create(EntityID owner)
{
	MeshRendererID ret = gMeshRendererManager.add();
	MeshRenderer& m = getMeshRenderer(ret);

	if(owner != -1)
	{
		m.transform = getEntity(owner)._transform;
	}
	else
	{
		m.transform = -1;
	}

	m.material = -1;
	m.mesh = -1;

	entity::addComponent(owner, ret, MESHRENDERER);

	return ret;
}

void meshrenderer::setMaterial(MeshRendererID mrId, MaterialID matID)
{
	MeshRenderer& r = getMeshRenderer(mrId);
	r.material = matID;
}

void meshrenderer::setMesh(MeshRendererID mrId, MeshID meshID)
{
	MeshRenderer& r = getMeshRenderer(mrId);
	r.mesh = meshID;
}

RenderKey meshrenderer::createRenderKey(MeshRenderer& renderer)
{
	RenderKey key;

	if(renderer.material != -1)
	{
		Material& mat = getMaterial(renderer.material);
		if(mat._flags & MAT_TRANSPARENT)
		{
			key.sortKey.transparent = 1;
		}
		else
		{
			key.sortKey.transparent = 0;
			key.sortKey.op_material = renderer.material;
			key.sortKey.op_shader = mat._shader;
		}

		key.shader = mat._shader;
		key.statedat = mat.states;
	}

	key.material = renderer.material;
	key.mesh = renderer.mesh;

	if(renderer.transform != -1)
	{
		key.transform = getTransform(renderer.transform)._matrix;
	}

	return key;
}

void meshrenderer::renderForView(CameraID cam)
{
	Camera& c = getCamera(cam);
	alfar::Vector3 camPos = transform::getWorldPosition(c._tn);

	for(int i = 0; i < gMeshRendererManager._num_objects; ++i)
	{
		MeshRenderer& m = gMeshRendererManager._objects[i];
		
		if(m.transform == -1 || m.material == -1 || m.mesh == -1)
			continue;//it's not fully set, ignore
		
		alfar::Vector3 objPos = transform::getWorldPosition(m.transform);

		RenderKey key = meshrenderer::createRenderKey(m);

		key.sortKey.transp_dist = alfar::vector3::sqrMagnitude(alfar::vector3::sub(camPos, objPos));
		key.sortKey.cameraID = cam;
		key.sortKey.layer = 5;//default layer for everything

		renderer::addRenderable(key);
	}
}