#include "Mesh.h"
#include "StringUtils.h"
#include "GLinclude.h"
#include "ResourceManager.h"
#include "DefaultGpuProgramLocations.h"
#include "MeshLoaderAssimp.h"

#include <fstream>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/packing.hpp>

#include <unordered_map>

Mesh::Mesh( const SharedPtr<VertexArrayObject> &vao, 
          const SharedPtr<GpuBuffer> &vertexBuffer, 
          const SharedPtr<GpuBuffer> &indexBuffer, 
          const std::vector<SubMesh> &subMeshes,
          const BoundingSphere &bounds ) :
    mVertexArrayObject(vao),
    mVertexBuffer(vertexBuffer),
    mIndexBuffer(indexBuffer),
    mSubMeshes(subMeshes),
    mBoundingSphere(bounds)
{
}

SharedPtr<VertexArrayObject> Mesh::getVertexArrayObject()
{
    return mVertexArrayObject;
}

SharedPtr<GpuBuffer> Mesh::getVertexBuffer()
{
    return mVertexBuffer;
}

SharedPtr<GpuBuffer> Mesh::getIndexBuffer()
{
    return mIndexBuffer;
}

const std::vector<SubMesh> &Mesh::getSubMeshes()
{
    return mSubMeshes;
}

SharedPtr<Mesh> Mesh::LoadMeshFromFile( const std::string &filename, 
                                        ResourceManager *resourceMgr )
{
    MeshLoaderAssimp loader;
    return loader.loadFile( filename );
}
