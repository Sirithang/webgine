#include "behaviour.h"
#include "array.h"
#include "memory.h"

#include <stdlib.h>

IMPLEMENT_MANAGED(Behaviour)

foundation::Array<BehaviourInterface*>* gSortedInterfaces;

int sortInterface(const void* a, const void* b)
{
	//we sort according to vtable to have all instance of the same class next to each other (cache friendly)
	//vtable being at index 0 we convert pointer to the interface instance to int an dereference it
	int* ba = (int*)a;
	int* bb = (int*)b;

	return *ba - *bb;
}

void behaviour::init()
{
	gSortedInterfaces = new foundation::Array<BehaviourInterface*>(foundation::memory_globals::default_scratch_allocator());
}


BehaviourID behaviour::create(EntityID ent)
{
	BehaviourID ret = gBehaviourManager.add();
	Behaviour& b = getBehaviour(ret);

	b._owner = ent;
	b._interface = 0;

	entity::addComponent(ent, ret, BEHAVIOUR);

	return ret;
}

void behaviour::setInterface(BehaviourID behave,BehaviourInterface* behaviourInterface)
{
	Behaviour& b = getBehaviour(behave);

	b._interface = behaviourInterface;

	if(b._interface)
	{
		foundation::array::push_back(*gSortedInterfaces, b._interface);
		qsort(gSortedInterfaces->_data, foundation::array::size(*gSortedInterfaces), sizeof(BehaviourInterface*), sortInterface);

		b._interface->_entity = b._owner;
		b._interface->created();
	}
}

void behaviour::update()
{
	BehaviourInterface** start = foundation::array::begin(*gSortedInterfaces);
	BehaviourInterface** end = foundation::array::end(*gSortedInterfaces);

	while(start != end)
	{
		(*start)->update(0.01f);

		start++;
	}
}