#include "input.h"
#include <stdio.h>

InputInfo _gInfo;

void input::init()
{
	emscripten_set_keypress_callback(0, 0, 1, input::emscript_key_callback);
	emscripten_set_keydown_callback(0, 0, 1, input::emscript_key_callback);
	emscripten_set_keyup_callback(0, 0, 1, input::emscript_key_callback);
}


EM_BOOL input::emscript_key_callback(int eventType, const EmscriptenKeyboardEvent *e, void *userData)
{
	switch(eventType)
	{
	case EMSCRIPTEN_EVENT_KEYPRESS:
		_gInfo._keys[e->keyCode] = PRESSED;
		break;
	case EMSCRIPTEN_EVENT_KEYDOWN:
		_gInfo._keys[e->keyCode] = DOWN;
		break;
	case EMSCRIPTEN_EVENT_KEYUP:
		_gInfo._keys[e->keyCode] = RELEASED;
		break;
	default:
		break;
	}

	return 1;
}

bool input::isKeyDown(const char key)
{
	std::map<char, KeyState>::iterator it = _gInfo._keys.find(key);
	if(it != _gInfo._keys.end())
	{
		return _gInfo._keys[key] == DOWN;
	}

	return false;
}
bool input::isKeyPressed(const char key)
{
	std::map<char, KeyState>::iterator it = _gInfo._keys.find(key);
	if(it != _gInfo._keys.end())
	{
		return _gInfo._keys[key] == PRESSED;
	}

	return false;
}
bool input::isKeyReleased(const char key)
{
	std::map<char, KeyState>::iterator it = _gInfo._keys.find(key);
	if(it != _gInfo._keys.end())
	{
		return _gInfo._keys[key] == RELEASED;
	}

	return false;
}
bool input::isKeyUp(const char key)
{
	std::map<char, KeyState>::iterator it = _gInfo._keys.find(key);
	if(it != _gInfo._keys.end())
	{
		return _gInfo._keys[key] == UP;
	}

	return false;
}