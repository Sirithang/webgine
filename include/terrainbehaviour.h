#pragma once

#include "behaviour.h"
#include "material.h"
#include "mesh.h"
#include "meshrenderer.h"

struct TerrainCreator : public BehaviourInterface
{
	MaterialID mat;
	MaterialID watMat;
	float accum;

	TextureID		skyboxTex;
	MaterialID		skyboxMat;
	MeshRendererID	skyboxMesh;

	TerrainCreator();

	virtual void created();
	virtual void update(float deltaTime);


	//============

	void spawnGrass(InputVertex* inputVert, int size);
	void createSkybox();
	void updateSkybox();
};