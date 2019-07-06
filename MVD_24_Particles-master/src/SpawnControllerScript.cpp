#include "SpawnControllerScript.h"
#include "extern.h"

void SpawnControllerScript::init() {
	//get transform of entity
	transform = &ECS.getComponentFromEntity<Transform>(owner_);

	//set spawn point to later know if there is one
	spawnRow = -1;
	spawnCol = -1;
	
	//set end point to later know if there is one
	endRow = -1;
	endCol = -1;

	//by default, the character can't move until the floor is ready
	move = false;

}

void SpawnControllerScript::update(float dt) {

	//look for spawn point when all tiles have been filed
	if (fs->placedAllTiles == true) {
		cout << "All tiles have been placed. Calculating route..." << endl;
		for (int i = 0; i < 12; i++) {
			for (int j = 0; j < 8; j++) {
				if (fs->floor_matrix[j][i] == 3) {
					spawnRow = j;
					spawnCol = i;
				}
				if (fs->floor_matrix[j][i] == 2) {
					endRow = j;
					endCol = i;
				}
			}
		}

		cout << "The Spawn point is in Row: " << spawnRow << " and in Column: " << spawnCol << endl;
		cout << "The Final point is in Row: " << endRow << " and in Column: " << endCol << endl;

		//if there's no spawn point or end point, player can't spawn or find an end so level needs to be redrawn
		if ((spawnCol == -1 && spawnRow == -1) || (endCol == -1 && endRow == -1 )) {
			
			cout << "No spawn point or end point has been found, the character can't move without them." << endl;
			cout << "Please redraw map with a start point (2) and an end point (1)" << endl;
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 12; j++) {
					int floor = ECS.getEntity("floor_" + to_string(j * 8 + i));
					Mesh& mesh = ECS.getComponentFromEntity<Mesh>(floor);
					mesh.material = 7;
				}
			}
		}else{

			//place teapot in start position
			float x = 15 - 2.5*spawnCol - 1.25;
			float z = - 10 - 2.5*spawnRow - 1.25;
			transform->m[12] = x;
			transform->m[13] = 0.0;
			transform->m[14] = z;
		}

		fs->placedAllTiles = false;
		move = true;
	}

	//place character in initial position
	if (fs->restart == true) {

		transform->m[12] = 0.0f;
		transform->m[13] = -1.0f;
		transform->m[14] = -20.0f;
	}


}