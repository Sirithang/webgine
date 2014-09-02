#pragma once

#include "objectmanager.h"
#include "entity.h"

//This interface is to be subclasse by component
struct BehaviourInterface
{
	EntityID _entity;

	virtual void created(){}
	virtual void update(float deltaTime);
};

//This is a class to have behaviour per entity
//Just supply a pointer to callbacks
struct Behaviour : public ManagedObject
{
	EntityID _owner;
	BehaviourInterface* _interface;
};

DECLARE_MANAGED(Behaviour, 2048)

namespace behaviour
{
	//call before any action, init global data
	void init();

	BehaviourID create(EntityID ent);

	void setInterface(BehaviourID behave,BehaviourInterface* behaviourInterface);

	void update();
}