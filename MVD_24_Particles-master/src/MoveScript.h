#pragma once
#include "includes.h"
#include "ScriptSystem.h"
#include "NavmeshScript.h"

class MoveScript : public Script {
public:
	MoveScript(int owner) : Script(owner) {};
	Transform *transform;
	NavmeshScript* nms;

	void init();
	void update(float dt);


};