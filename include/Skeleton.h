#pragma once

#include <glm/mat4x4.hpp>

#include <vector>

#include "Animation.h"

struct Bone {
    unsigned int parent;
    glm::mat4 boneMatrix,    // model -> bone space
              invBoneMatrix; // bone -> model space
          
};


class SkeletonInstance;
class Skeleton
{
public:
    Skeleton( const std::vector<Bone> &bones ) :
        mBones(bones)
    {}
    
    void setAnimationClip( const std::string &name, AnimationClip clip ) {
        mAnimations[name] = std::move(clip);
    }
    
    SharedPtr<SkeletonInstance> createInstance();
    
    AnimationClip& getAnimation( const std::string &name );
    
private:
    std::vector<Bone> mBones;
    std::map<std::string,AnimationClip> mAnimations;
};

class SkeletonInstance {
    friend class Skeleton;
    
protected:
    SkeletonInstance( Skeleton *skeleton );
    
public:
    void update( float dt );
    void playAnimation( const std::string &name );
    
private:
    Skeleton *mSkeleton;
    SharedPtr<AnimationInstance> mAnimation;
    std::vector<glm::mat4> mBoneTransforms;
};

