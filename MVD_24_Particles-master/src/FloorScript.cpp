#include "FloorScript.h"
#include "extern.h"


void FloorScript::init() {
	
	//set up ground matrix
	Vec rows(colSize);

	for (int i = 0; i < rowSize; i++) {
		for (int j = 0; j < colSize; j++) {
			floor_matrix.push_back(rows);
			floor_matrix[i][j] = -1;
		}
	}
	
	//initialize control grid variables
	currentCol = 0;
	currentRow = 0;
	restart = false;
	placedAllTiles = false;
	previousPressed = false;
	currentPressed = false;
	transform = &ECS.getComponentFromEntity<Transform>(owner_);

}

void FloorScript::update(float dt) {
	
	//make one action per each time a button is pressed and not as many times as update goes while the button is pressed
	previousPressed = currentPressed;

	//react only when correct input for registering a material is correct
	if(input_->GetKey(GLFW_KEY_1) || input_->GetKey(GLFW_KEY_2) || input_->GetKey(GLFW_KEY_3) || input_->GetKey(GLFW_KEY_4) 
		|| input_->GetKey(GLFW_KEY_R)){
		currentPressed = true;

		//get material the user wants according the button pressed
		if(input_->GetKey(GLFW_KEY_1)) {
			mat_value = 2;
		}
		if (input_->GetKey(GLFW_KEY_2)) {
			mat_value = 3;
		}
		if (input_->GetKey(GLFW_KEY_3)) {
			mat_value = 4;
		}
		if (input_->GetKey(GLFW_KEY_4)) {
			mat_value = 5;
		}

		//user can restart painting the grid and restart the simulation
		if (input_->GetKey(GLFW_KEY_R)) {
			currentPressed = true;
			restart = true;
			placedAllTiles = false;
			mat_value = 6;
			currentCol = 0;
			currentRow = 0;
			for (int i = 0; i < 12; i++) {
				for (int j = 0; j < 8; j++) {
					int floor = ECS.getEntity("floor_" + to_string(i * 8 + j));
					Mesh& mesh = ECS.getComponentFromEntity<Mesh>(floor);
					mesh.material = 6;
					floor_matrix[j][i] = -1;
				}
			}
		}
	}else{
		currentPressed = false;
		restart = false;
	}

	//perform registered action once per click
	if (previousPressed == false && currentPressed == true) {

		//check if restarted
		if (restart) {
			cout << "Restarting grid selection. Please start drawing from the beginning the desired design" << endl;
		}

		//making sure it doesn't go beyond all the available tiles
		if (currentCol * 8 + currentRow < 96 && mat_value != 6) {

			//change material from tile for better UX
			int floor = ECS.getEntity("floor_" + to_string(currentCol * 8 + currentRow));
			Mesh& mesh = ECS.getComponentFromEntity<Mesh>(floor);
			mesh.material = mat_value;
			
			//save floor status according to grid
			switch (mat_value) {
				case 2: //case end of route
					floor_matrix[currentRow][currentCol] = 2;
					break;
				case 3: //case start of route
					floor_matrix[currentRow][currentCol] = 3;
					break;
				case 4: //case route
					floor_matrix[currentRow][currentCol] = 1;
					break;
				case 5: //case bad floor
					floor_matrix[currentRow][currentCol] = 0;
					break;
			}
			
			cout << "You are in Row: " << currentRow << " and Column: " << currentCol << " of the floor matrix" << endl;
			cout << "You have selected a tile type: " << mat_value + 1 << endl;
			
			//checking if the end of a row or column is reached and stepping into the next one properly
			if (currentCol <= 11) {
				if (currentRow == 7) {
					currentCol++;
					currentRow = 0;
				}else {
					currentRow++;
				}
			}
		}else {
			//if all tiles are placed, we can start navigating the floor grid
			if (!restart) {
				placedAllTiles = true;
				currentCol = 0;
				currentRow = 0;
			}
		}
	}



}