#ifndef FloorScript_HEADER
#define FloorScript_HEADER
#include "includes.h"
#include "ScriptSystem.h"
#include "extern.h"

typedef vector<int> Vec;
typedef vector<Vec> Mat;

class FloorScript : public Script {
public:
	FloorScript(int owner) : Script(owner) {};
	Transform *transform;

	void init();
	void update(float dt);

	//variables to control floor grid
	const int rowSize = 8;
	const int colSize = 12;
	Mat floor_matrix;
	int currentRow;
	int currentCol;

	//store material value of each floor grid according to user input
	int mat_value;

	//bools to control restart, pressed keys and if all tiles are placed
	bool restart;
	bool move;
	bool placedAllTiles;
	bool currentPressed;
	bool previousPressed;
};

#endif