#pragma once
#include "includes.h"
#include "ScriptSystem.h"
#include "FloorScript.h"

class SpawnControllerScript : public Script {
public:
	SpawnControllerScript(int owner) : Script(owner) {};
	Transform *transform;
	FloorScript* fs;

	//spawn point
	int spawnCol;
	int spawnRow;

	//end point
	int endCol;
	int endRow;

	//mark character can is ready to move
	int move;

	void init();
	void update(float dt);


};