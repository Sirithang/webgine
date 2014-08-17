#pragma once

/**
	This was inspired by http://bitsquid.blogspot.co.uk/search?q=decoupling
*/

#include <limits.h>
#include "types.h"

typedef unsigned ID;

#define INDEX_MASK 0xffff
#define NEW_OBJECT_ID_ADD 0x10000

struct Index {
	ID id;
	unsigned short index;
	unsigned short next;
};


struct ManagedObject
{
	ID id;
};

/**
* This allow to allocate objects as contiguous memory space. This expand the array
* by memmove, so never stock inside something needing constructeur/destructor to be moved.
* ID 0 is always an empty unitialized object, so you can use 0 as the "dafualt no-id"
* Stocked object need to have an .id field to stock the array id.
*/
template<typename T, uint32_t MAX_OBJECTS>
struct ObjectManager
{
	unsigned _num_objects;
	T _objects[MAX_OBJECTS];
	Index _indices[MAX_OBJECTS];
	unsigned short _freelist_enqueue;
	unsigned short _freelist_dequeue;

	ObjectManager() 
	{
		_num_objects = 0;
		for (unsigned i=0; i<MAX_OBJECTS; ++i) 
		{
			_indices[i].id = i;
			_indices[i].next = i+1;
		}
		_freelist_dequeue = 0;
		_freelist_enqueue = MAX_OBJECTS-1;
	}

	inline bool has(ID id) 
	{
		Index &in = _indices[id & INDEX_MASK];
		return in.id == id && in.index != USHRT_MAX;
	}
	
	inline T &lookup(ID id) 
	{
		return _objects[_indices[id & INDEX_MASK].index];
	}
	
	inline ID add() 
	{
		Index &in = _indices[_freelist_dequeue];
		_freelist_dequeue = in.next;
		in.id += NEW_OBJECT_ID_ADD;
		in.index = _num_objects++;
		T &o = _objects[in.index];
		o.id = in.id;
		return o.id;
	}
	
	inline void remove(ID id) 
	{
		Index &in = _indices[id & INDEX_MASK];
		
		T &o = _objects[in.index];
		o = _objects[--_num_objects];
		_indices[o.id & INDEX_MASK].index = in.index;
		
		in.index = USHRT_MAX;
		_indices[_freelist_enqueue].next = id & INDEX_MASK;
		_freelist_enqueue = id & INDEX_MASK;
	}

	//Shortcut to add and retrieve the object directly. NEVER STOCK REFERENCE OR POINTER of returned object, can move.
	inline T& addAndGet()
	{
		ID ret = add();
		return lookup(ret);
	}
};


//Macro a bit complexe (don't really like those but reduce copy-pasting
//It just declare manager for the given type (just allow to have a specific manager class per managed type)
//Also declare alias for ID to the given type. allow for cleaner code (ex EntityID design ONLY id for Entity, not other managed type)
//Finally declare a function to query a certain managed type in the corresponding manager
#define DECLARE_MANAGED(Type, InitialMax) \
	struct Type##Manager : public ObjectManager<Type,InitialMax>{}; \
	extern Type##Manager g##Type##Manager; \
	typedef ID Type##ID; \
	inline Type& get##Type(Type##ID id) { return g##Type##Manager.lookup(id);}

//this is just used to implement the actual manager
#define IMPLEMENT_MANAGED(Type) \
	Type##Manager g##Type##Manager;