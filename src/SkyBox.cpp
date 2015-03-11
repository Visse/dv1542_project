#include "SkyBox.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Mesh.h"

SkyBox::SkyBox( SceneObjectFactory *factory, Root *root, const SharedPtr<Material> &material ) :
    SceneObject(factory),
    mRoot(root),
    mMaterial(material)
{
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mMesh = resourceMgr->getMeshAutoPack("Cube");
    
    BoundingSphere bounds( glm::vec3(), std::numeric_limits<float>::infinity() );
    setBoundingSphere( bounds );
}
