#include "Skeleton.h"
#include "Animation.h"

SharedPtr<SkeletonInstance> Skeleton::createInstance()
{
    return makeSharedPtr<SkeletonInstance>( this );
}

SkeletonInstance::SkeletonInstance( Skeleton *skeleton ) :
    mSkeleton(skeleton)
{
}

void SkeletonInstance::playAnimation( const std::string &name )
{
    mSkeleton->getAnimation(name);
}

void SkeletonInstance::update( float dt )
{
    if( mAnimation ) {
        mAnimation->update( dt )
    }
}




