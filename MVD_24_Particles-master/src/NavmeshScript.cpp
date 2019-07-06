#include "NavmeshScript.h"
#include "extern.h"

void NavmeshScript::init() {
	//get transform of entity
	transform = &ECS.getComponentFromEntity<Transform>(owner_);

	//set up direction, we start on the left because of the initial direction of the teapot's handle
	direction = 2;
	prevdir = 2;

	//set current character to default value
	currCol = -1;
	currRow = -1;
	prevCol = -1;
	prevRow = -1;

	//by default, we don't have a spawn point until the first move iteration
	spawnPoint = false;
	cantMove = false;
	move = false;
}

void NavmeshScript::update(float dt) {

	//if the floor is marked as correct, the character can start moving
	if (scs->move) {
		
		//setting previous position to know where the character comes from to set a direction
		cout << "Character currently in Col: " << currCol << " and in Row: " << currRow << endl;	
		prevCol = currCol;
		prevRow = currRow;

		if (!move) {

			//get spawn position
			if (!spawnPoint) {
				currCol = scs->spawnCol;
				currRow = scs->spawnRow;
				spawnPoint = true;
			}
			if (fs->floor_matrix[currRow][currCol] == 2) {
				cout << "Character has found the end of your trail succesfully!" << endl;
				float x = 15 - 2.5*currCol - 1.25;
				float z = 10 - 2.5*currRow - 1.25;
				transform->m[12] = x;
				transform->m[13] = 0.0;
				transform->m[14] = z;
				scs->move = false;
			}
			else {
				//move according to direction
				switch (direction) {
					//move up
				case 0:
					//check if we are at the end of the grid from the top
					if (currRow - 1 >= 0) {
						//check if the character can move upwards
						if (fs->floor_matrix[currRow - 1][currCol] == 1 || fs->floor_matrix[currRow - 1][currCol] == 2) {
							currRow = currRow - 1;
						}
						else {
							//check if it can move to the left && the left tile is a floor tile
							if (currCol - 1 >= 0) {
								if (fs->floor_matrix[currRow][currCol - 1] == 1 || fs->floor_matrix[currRow][currCol - 1] == 2) {
									direction = 2;
									currCol = currCol - 1;
								}
								else {
									//check if it can move to the right
									if (currCol + 1 <= 11) {
										if (fs->floor_matrix[currRow][currCol + 1] == 1 || fs->floor_matrix[currRow][currCol + 1] == 2) {
											direction = 3;
											currCol = currCol + 1;
										}
										else {
											//check if only direction is backwards
											if (currRow + 1 <= 7) {
												if (fs->floor_matrix[currRow + 1][currCol] == 1 || fs->floor_matrix[currRow + 1][currCol] == 2) {
													direction = 1;
													currRow = currRow + 1;
												}
											}
										}
									}
								}
							}
						}
					}
					move = true;
					break;
					//move down
				case 1:
					//check if we are at the end of the grid from the bottom
					if (currRow + 1 <= 7) {
						//check if the character can move downwards
						if (fs->floor_matrix[currRow + 1][currCol] == 1 || fs->floor_matrix[currRow + 1][currCol] == 2) {
							currRow = currRow + 1;
						}else {
							//check if it can move to the left && the left tile is a floor tile
							if (currCol - 1 >= 0) {
								if (fs->floor_matrix[currRow][currCol - 1] == 1 || fs->floor_matrix[currRow][currCol - 1] == 2) {
									direction = 2;
									currCol = currCol - 1;
								}else {
									//check it can move right && the right tile is a floor tile
									if (currCol + 1 <= 11) {
										if (fs->floor_matrix[currRow][currCol + 1] == 1 || fs->floor_matrix[currRow][currCol + 1] == 2) {
											direction = 3;
											currCol = currCol + 1;
										}else {
											//check if only direction is backwards
											if (currRow - 1 >= 0) {
												if (fs->floor_matrix[currRow - 1][currCol] == 1 || fs->floor_matrix[currRow - 1][currCol] == 2) {
													direction = 0;
													currRow = currRow - 1;
												}
											}
										}
									}
								}
							}
						}
					}
					move = true;
					break;
					//move left
				case 2:
					//check if we are at the end of the grid from the left
					if (currCol - 1 >= 0) {
						//check if the character can move to the left
						if (fs->floor_matrix[currRow][currCol - 1] == 1 || fs->floor_matrix[currRow][currCol - 1] == 2) {
							currCol = currCol - 1;
						}
						else {
							//check if it can move up && the up position is a floor tile
							if (currRow - 1 >= 0) {
								if (fs->floor_matrix[currRow - 1][currCol] == 1 || fs->floor_matrix[currRow - 1][currCol] == 2) {
									direction = 0;
									currRow = currRow - 1;
								}else {
									//check it can move down && the down position is a floor tile
									if (currRow + 1 <= 7) {
										if (fs->floor_matrix[currRow + 1][currCol] == 1 || fs->floor_matrix[currRow + 1][currCol] == 2) {
											direction = 1;
											currRow = currRow + 1;
										}
										else {
											//check if direction is to the right
											if (currCol + 1 <= 11) {
												if (fs->floor_matrix[currRow][currCol + 1] == 1 || fs->floor_matrix[currRow][currCol + 1] == 2) {
													direction = 3;
													currCol = currCol + 1;
												}
											}
										}
									}
								}
							}
						}
					}
					move = true;
					break;
					//move right
				case 3:
					//check if we are at the end of the grid from the left
					if (currCol + 1 <= 11) {
						//check if the character can move to the left
						if (fs->floor_matrix[currRow][currCol + 1] == 1 || fs->floor_matrix[currRow][currCol + 1] == 2) {
							currCol = currCol + 1;
						}else {
							//check if it can move up && the up position is a floor tile
							if (currRow - 1 >= 0) {
								if (fs->floor_matrix[currRow - 1][currCol] == 1 || fs->floor_matrix[currRow - 1][currCol] == 2) {
									direction = 0;
									currRow = currRow - 1;
								}else {
									//check it can move down && the down position is a floor tile
									if (currRow + 1 <= 7) {
										if (fs->floor_matrix[currRow + 1][currCol] == 1 || fs->floor_matrix[currRow + 1][currCol] == 2) {
											direction = 1;
											currRow = currRow + 1;
										}else{
											//check if direction is to the left
											if (currCol - 1 >= 0) {
												if (fs->floor_matrix[currRow][currCol - 1] == 1 || fs->floor_matrix[currRow][currCol - 1] == 2) {
													direction = 2;
													currCol = currCol - 1;
												}
											}
										}
									}
								}
							}
						}
					}
					move = true;
					break;
				default:
					break;
				}
			}
		}
	}

	//if the floor is restarted, the spawn point will have to be taken again
	if (fs->restart) {
		spawnPoint = false;
	}

}