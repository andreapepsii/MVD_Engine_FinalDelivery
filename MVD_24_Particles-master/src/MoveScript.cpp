#include "MoveScript.h"
#include "extern.h"

void MoveScript::init() {
	//get transform of entity
	transform = &ECS.getComponentFromEntity<Transform>(owner_);
}

void MoveScript::update(float dt) {

	//if the position of the character has changed, it will move to that next spot
	if (nms->move) {

		//avoid moving to -1 -1, which is the start value
		if ((nms->prevCol != -1) && (nms->prevCol != -1)) {
			float x = 15 - 2.5*nms->currCol - 1.25;
			float z = 10 - 2.5*nms->currRow - 1.25;
			transform->m[12] = x;
			transform->m[13] = 0.0;
			transform->m[14] = z;
		}
		
		//movement is done and it will need to calculate next position
		nms->move = false;
		
	}
}
