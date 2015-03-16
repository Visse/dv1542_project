#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "SharedPtr.h"

class Skeleton;

struct AnimationSample {
    glm::quat rotation;
    glm::vec3 position;
    
    float timeStamp = 0.f;
};

struct AnimationChannel {
    std::vector<AnimationSample> samples;
};

struct AnimationClip {
    std::string name;
    std::vector<AnimationChannel> channels;
    
    float lenght = 0.f;
};

class AnimationInstance {
public:
    AnimationInstance( AnimationClip *clip );
    void update( float dt );
    
private:
    AnimationClip *mClip = nullptr;
    std::vector<AnimationSample> mCurrentSamples;
    
    float mCurrentTime = 0.f;
};