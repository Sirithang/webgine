#include "entity.h"
#include "transform.h"
#include "array.h"

IMPLEMENT_MANAGED(Entity)



EntityID entity::create()
{
	EntityID id = gEntityManager.add();

	Entity& ent = getEntity(id);

	ent._componentCount = 0;

	//add the default transform that every entity own
	ent._transform = transform::create(id);

	return id;
}

void entity::addComponent(EntityID ent, ID compID, ComponentType type)
{
	Entity& e = getEntity(ent);
	Entity::ComponentBind bind;

	bind._type = type;
	bind._component = compID;

	e._components[e._componentCount] = bind;
	e._componentCount += 1;
}

ID entity::getComponent(EntityID id, ComponentType type)
{
	Entity& ent = getEntity(id);
	
	for(int i = 0; i < ent._componentCount; ++i)
	{
		if(ent._components[i]._type == type)
			return ent._components[i]._component;
	}

	return -1;
}