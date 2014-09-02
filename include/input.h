#pragma once

//TODO replace that
#include <map>

#include <html5.h>


enum KeyState
{
	PRESSED,
	DOWN,
	UP,
	RELEASED
};

struct InputInfo
{
	std::map<char, KeyState> _keys;
};

namespace input
{
	void init();

	EM_BOOL emscript_key_callback(int eventType, const EmscriptenKeyboardEvent *keyEvent, void *userData);

	bool isKeyDown(const char key);
	bool isKeyPressed(const char key);
	bool isKeyReleased(const char key);
	bool isKeyUp(const char key);
}