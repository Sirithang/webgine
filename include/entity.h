#pragma once

#include "enums.h"
#include "collection_types.h"
#include "objectmanager.h"

struct Entity : ManagedObject
{
	struct ComponentBind
	{
		ID _component;
		ComponentType _type;
	};

	//Entity ALWAYS own a transform, this is a simple shortcut to it.
	ID _transform;

	ComponentBind _components[16];
	int _componentCount;
};


DECLARE_MANAGED(Entity, 1024)

namespace entity
{
	EntityID create();

	 void addComponent(EntityID ent, ID compID, ComponentType type);
	 ID getComponent(EntityID ent, ComponentType type);
}