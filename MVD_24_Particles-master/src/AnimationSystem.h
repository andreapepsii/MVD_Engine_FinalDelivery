#pragma once
#include "includes.h"
#include "Shader.h"
#include "Components.h"

class AnimationSystem {
public:
    ~AnimationSystem();
    void init();
    void lateInit();
    void update(float dt);
    
private:
    GLuint curr_frame_ = 0;
    float ms_per_frame_ = 41.666f;
    float ms_counter_anim = 0;
	float ms_counter_skin = 0;
    
    void incrementJointFrame_(Joint* joint);
	void restartJoints(Joint* joint);
    
    void deformBlendShapes_();
    
};
