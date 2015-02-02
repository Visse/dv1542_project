#pragma once

#include "UniquePtr.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class Octree;
class SceneZone;
class SceneObjects;
class SceneObject;

struct Portal {
    SceneZone *toZone;
    
    glm::vec3 position, size;
    glm::quat orientation;
};

class SceneZone {
public:
    SceneZone();
    
    void addObject( SceneObject *object );
    uint addPortal( SceneZone *toZone );
    
private:
    UniquePtr<Octree> mOctree;
    UniquePtr<Portal> mPortals;
};