#pragma once

#include <yaml-cxx/Node.h>

class Root;
class SceneObject;

class SceneObjectFactory {
public:
    SceneObjectFactory() = default;
    virtual ~SceneObjectFactory() = default;
    
    SceneObjectFactory( const SceneObjectFactory& ) = delete;
    SceneObjectFactory( SceneObjectFactory&& ) = delete;
    SceneObjectFactory& operator = ( const SceneObjectFactory& ) = delete;
    SceneObjectFactory& operator = ( SceneObjectFactory&& ) = delete;
    
    virtual SceneObject *createObject( const Yaml::Node &node ) = 0;
    virtual void destroyObject( SceneObject *object );
};


class DeferredEntityFactory :
    public SceneObjectFactory
{
public:
    DeferredEntityFactory( Root *root );
    virtual SceneObject* createObject( const Yaml::Node &node ) override;
    
    
private:
    Root *mRoot;
};

class ComputeParticleFactory :
    public SceneObjectFactory
{
public:
    ComputeParticleFactory( Root *root );
    virtual SceneObject* createObject( const Yaml::Node& node ) override;
    
private:
    Root *mRoot;
};

class LightFactory :
    public SceneObjectFactory
{
public:
    LightFactory( Root *root );
    virtual SceneObject *createObject( const Yaml::Node& node ) override;
    
private:
    Root *mRoot;
};

class SkyBoxFactory :
    public SceneObjectFactory
{
public:
    SkyBoxFactory( Root *root );
    virtual SceneObject* createObject( const Yaml::Node& node ) override;
    
private:
    Root *mRoot;
};
