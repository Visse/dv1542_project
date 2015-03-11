#include "SkyBox.h"
#include "Root.h"
#include "ResourceManager.h"
#include "Mesh.h"

SkyBox::SkyBox( Root *root, const SharedPtr<Material> &material ) :
    mRoot(root),
    mMaterial(material)
{
    ResourceManager *resourceMgr = mRoot->getResourceManager();
    mMesh = resourceMgr->getMeshAutoPack("Cube");
    
    BoundingSphere bounds( glm::vec3(), std::numeric_limits<float>::infinity() );
    setBoundingSphere( bounds );
}
