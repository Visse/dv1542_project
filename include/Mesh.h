#pragma once

#include "GpuBuffer.h"
#include "SharedPtr.h"
#include "VertexArrayObject.h"
#include "BoundingSphere.h"

#include <string>
#include <vector>
#include <map>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class ResourceManager;
class Material;

struct SubMesh {
    size_t vertexStart, vertexCount;
    SharedPtr<Material> material;
};

class Mesh {
public:
    static SharedPtr<Mesh> LoadMeshFromFile( const std::string &filename,
                                             ResourceManager *resourceMgr );
    
    struct Vertex {
        glm::vec3 position,
                  normal,
                  tangent,
                  bitangent;
        glm::vec2 texcoord;
    };
public:
    Mesh( const SharedPtr<VertexArrayObject> &vao, 
          const SharedPtr<GpuBuffer> &vertexBuffer, 
          const SharedPtr<GpuBuffer> &indexBuffer, 
          const std::vector<SubMesh> &subMeshes,
          const BoundingSphere &bounds
        );
    
    SharedPtr<VertexArrayObject> getVertexArrayObject();
    SharedPtr<GpuBuffer> getVertexBuffer();
    SharedPtr<GpuBuffer> getIndexBuffer();
    
    const std::vector<SubMesh>& getSubMeshes();
    
    void setName( const std::string &name ) {
        mName = name;
    }
    const std::string& getName() {
        return mName;
    }
    const BoundingSphere& getBoundingSphere() {
        return mBoundingSphere;
    }
    
private:
    SharedPtr<VertexArrayObject> mVertexArrayObject;
    SharedPtr<GpuBuffer> mVertexBuffer, mIndexBuffer;
    
    std::vector<SubMesh> mSubMeshes;
    
    std::string mName;
    BoundingSphere mBoundingSphere;
};