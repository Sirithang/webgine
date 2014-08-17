#pragma once


//new component should be registered there for easy identification
enum ComponentType
{
	TRANSFORM	= 1, //0 is used as "NO TYPE"
	CAMERA		= 2,

	//Engine extensions can potentially declare component after 255
	MAX_CORE_COMPONENT = 255
};