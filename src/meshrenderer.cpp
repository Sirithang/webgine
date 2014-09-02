#include "meshrenderer.h"
#include "renderer.h"
#include "vector3.h"
#include <stdio.h>

IMPLEMENT_MANAGED(MeshRenderer);

MeshRendererID meshrenderer::create(EntityID owner)
{
	MeshRendererID ret = gMeshRendererManager.add();
	MeshRenderer& m = getMeshRenderer(ret);

	m.transform = getEntity(owner)._transform;

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

void meshrenderer::renderForView(CameraID cam)
{
	Camera& c = getCamera(cam);
	alfar::Vector3 camPos = transform::getWorldPosition(c._tn);

	for(int i = 0; i < gMeshRendererManager._num_objects; ++i)
	{
		MeshRenderer& m = gMeshRendererManager._objects[i];
		Material& mat = getMaterial(m.material);
		RenderKey key;
		
		alfar::Vector3 objPos = transform::getWorldPosition(m.transform);

		key.sortKey.cameraID = cam;

		if(mat._flags & MAT_TRANSPARENT)
		{
			key.sortKey.transparent = 1;
			key.sortKey.transp_dist = alfar::vector3::sqrMagnitude(alfar::vector3::sub(camPos, objPos));
		}
		else
		{
			key.sortKey.transparent = 0;
			key.sortKey.op_material = m.material;
			key.sortKey.op_shader = mat._shader;
		}

		key.material = m.material;
		key.shader = mat._shader;

		//printf("adding key with camera : %i, material : %i, shader : %i \n", key.sortKey.cameraID, key.sortKey.materialID, key.sortKey.shaderID);
		
		key.mesh = m.mesh;
		key.transform = getTransform(m.transform)._matrix;

		renderer::addRenderable(key);
	}
}