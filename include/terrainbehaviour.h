#pragma once

#include "behaviour.h"
#include "material.h"
#include "mesh.h"

struct TerrainCreator : public BehaviourInterface
{
	MaterialID mat;
	MaterialID watMat;
	float accum;

	TerrainCreator();

	virtual void created();
	virtual void update(float deltaTime);


	//============

	void spawnGrass(InputVertex* inputVert, int size);
};