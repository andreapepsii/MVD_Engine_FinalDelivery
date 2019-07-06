#pragma once
#include "includes.h"
#include "ScriptSystem.h"
#include "FloorScript.h"
#include "SpawnControllerScript.h"

class NavmeshScript : public Script {
public:
	NavmeshScript(int owner) : Script(owner) {};
	Transform *transform;
	FloorScript* fs;
	SpawnControllerScript* scs;
	
	//character direction
	int direction; //0 -> up, 1 -> down, 2 -> left, 3 -> right
	int prevdir;

	//keep in track what area of the floor the character is
	int currCol;
	int currRow;
	int prevCol;
	int prevRow;

	//get spawn position
	bool spawnPoint;

	//bool to mark player can't move anymore
	bool cantMove;
	bool move;

	void init();
	void update(float dt);


};