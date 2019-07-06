//
//  AnimationSystem.cpp
//

#include "AnimationSystem.h"
#include "extern.h"

//destructor
AnimationSystem::~AnimationSystem() {

}

//set initial state 
void AnimationSystem::init() {

}

//called after loading everything
void AnimationSystem::lateInit() {

    
}

void AnimationSystem::update(float dt) {
    
	bool trigger_frame = false;
	bool trigger_frame2 = false;

	//increment millisecond counter
	ms_counter_anim += dt * 1000;
	ms_counter_skin += dt * 1000;

    //animation component
    auto& anims = ECS.getAllComponents<Animation>();
    for (auto& anim : anims) {
		
		//if counter above threshold
		if (ms_counter_anim >= anim.ms_frame) {
			trigger_frame = true;
			ms_counter_anim = 0;
		}

        //get transform
        Transform& transform = ECS.getComponentFromEntity<Transform>(anim.owner);
        //if new frame
        if (trigger_frame && anim.active) {
            //set positions to current frame
            transform.set(anim.keyframes[anim.curr_frame]);
            //advance frame
            anim.curr_frame++;
            //loop if required
            if (anim.curr_frame == anim.num_frames)
                anim.curr_frame = 0;
        }
    }
    
    //skinned mesh joints
    auto& skinnedmeshes = ECS.getAllComponents<SkinnedMesh>();
	for (auto& sm : skinnedmeshes) {
		if (!sm.root) continue; //only if mesh has a joint chain!
		//if counter above threshold
		if (ms_counter_skin >= sm.ms_frame) {
			trigger_frame2 = true;
			ms_counter_skin = 0;
		}
		if (trigger_frame2 && sm.active == 1) {
			incrementJointFrame_(sm.root);
		}
		if (sm.active == -1) {
			restartJoints(sm.root);
			sm.active = 1;
		}
    }
    
    deformBlendShapes_();
}

void AnimationSystem::incrementJointFrame_(Joint* joint) {
    
    //avoid updating joints that don't have any keyframes
    if (joint->num_keyframes > 0) {
        
        joint->current_keyframe++;
        if (joint->current_keyframe == joint->num_keyframes)
            joint->current_keyframe = 0;
        
        joint->matrix = joint->keyframes[joint->current_keyframe] ;
    }
    for (auto& c : joint->children) {
        incrementJointFrame_(c);
    }
}

void AnimationSystem::restartJoints(Joint* joint) {
	//avoid updating joints that don't have any keyframes
	if (joint->num_keyframes > 0) {

		joint->current_keyframe = 0;
		joint->matrix = joint->keyframes[joint->current_keyframe];
	}
	for (auto& c : joint->children) {
		restartJoints(c);
	}
}

void AnimationSystem::deformBlendShapes_() {
    auto& blend_components = ECS.getAllComponents<BlendShapes>();
    for (auto& blend_comp : blend_components) {
        //check entity has parent mesh
        if (!ECS.hasComponent<Mesh>(blend_comp.owner)) {
            std::string error_msg = "ERROR: entity " + ECS.getEntityName(blend_comp.owner) + " does not have a mesh, can't blend!";
            print(error_msg);
            continue;
        }
        
        //do blending here
    }
}
