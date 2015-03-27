#include "Scene.h"
#include "SceneObject.h"
#include "Root.h"
#include "SharedEnums.h"
#include "Frustrum.h"
#include "SceneGraph.h"
#include "UniformBlockDefinitions.h"
#include "SceneLoader.h"

#include <cassert>
#include <algorithm>

#include <iostream>

SharedPtr<Scene> Scene::LoadFromFile( Root *root, const std::string &filename )
{
    SceneLoader loader( root );
    loader.loadFile( filename );
    return loader.getScene();
}

Scene::Scene( Root *root ):
    mRoot(root)
{
    mSceneGraph = makeUniquePtr<SceneGraph>(mRoot);
}

void Scene::addObject( SceneObject *object, bool takeOwnership )
{
    if( object->isDirty() ) {
        object->_updateTransform();
    }
    object->_setAutoDelete( takeOwnership );
    mSceneGraph->addObject( object );
}

void Scene::removeObject( SceneObject *object )
{
    mSceneGraph->removeObject( object );
}

void Scene::update( float dt )
{
    mSceneGraph->update( dt );
}

void Scene::quarySceneObjects( const Frustrum &frustrum, std::vector<SceneObject*> &result )
{
    mSceneGraph->quaryObjects( frustrum, result );
}

void Scene::forEachObject( const std::function<void(SceneObject*)> &callback )
{
    mSceneGraph->forEachObject( callback );
}

AmbientUniforms Scene::getAmbientUniforms()
{
    AmbientUniforms uniforms;
    uniforms.color = mAmbientColor;
    return uniforms;
}
