#pragma once

#include "objectmanager.h"
#include "entity.h"
#include "mesh.h"
#include "transform.h"
#include "camera.h"
#include "material.h"

struct MeshRenderer : public ManagedObject
{
	MeshID mesh;
	MaterialID material;

	TransformID transform;
};

DECLARE_MANAGED(MeshRenderer, 1024)

namespace meshrenderer
{
	MeshRendererID create(EntityID owner);

	void setMesh(MeshRendererID id, MeshID meshID);
	void setMaterial(MeshRendererID id, MaterialID matID);

	void renderForView(CameraID cam);
};