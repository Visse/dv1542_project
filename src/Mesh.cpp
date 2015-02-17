#include "Mesh.h"
#include "StringUtils.h"
#include "GLinclude.h"
#include "ResourceManager.h"
#include "DefaultGpuProgramLocations.h"
#include "MeshLoaderWavefront.h"

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

void generateTangentAndBitangent( 
    std::vector<Mesh::Vertex> &vertexes,
    const std::vector<GLuint> &indexes
);

SharedPtr<Mesh> uploadMeshToGpu(
    const std::vector<Mesh::Vertex> &vertexes,
    const std::vector<GLuint> &indexes,
    const std::vector<SubMesh> &submeshes,
    const BoundingSphere &bounds
);

SharedPtr<Mesh> Mesh::LoadMeshFromFile( const std::string &filename, 
                                        ResourceManager *resourceMgr )
{
    MeshLoaderWavefront loader( resourceMgr );
    loader.loadFile( filename );
    
    const auto &vertexes = loader.getVertexes();
    const auto &indexes = loader.getIndexes();
    const auto &submeshes = loader.getSubmeshes();
    
    std::vector<Vertex> vert;
    vert.reserve( vertexes.size() );
    std::transform( vertexes.begin(), vertexes.end(), std::back_inserter(vert),
        []( const MeshLoaderWavefront::Vertex &vertex ) {
            Vertex v;
                v.position = vertex.position;
                v.normal   = vertex.normal;
                v.texcoord = vertex.texcoord;
            return v;
        }
    );
    
    generateTangentAndBitangent( vert, indexes );
    return uploadMeshToGpu( vert, indexes, submeshes, loader.getBounds() );
}

void generateTangentAndBitangent( std::vector<Mesh::Vertex> &vertexes, 
                                  const std::vector< GLuint > &indexes )
{
    // formula from here:
    // http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-13-normal-mapping/
    
    typedef Mesh::Vertex Vertex;

    for( size_t i=0, c = indexes.size(); i < c; i += 3 ) {
        
        Vertex v[3] = {
            vertexes.at(indexes[i+0]),
            vertexes.at(indexes[i+1]),
            vertexes.at(indexes[i+2])
        };
        
        glm::vec3 dPos[2] = {
            v[1].position - v[0].position,
            v[2].position - v[0].position
        };
        glm::vec2 dTex[2] = {
            v[1].texcoord - v[0].texcoord,
            v[2].texcoord - v[0].texcoord
        };
        
        float r = 1.0f / (dTex[0].x * dTex[1].y - dTex[0].y * dTex[1].x);
        glm::vec3 tangent = (dPos[0] * dTex[1].y - dPos[1] * dTex[0].y) * r;
        glm::vec3 bitangent = (dPos[1] * dTex[0].x - dPos[0] * dTex[1].x) * r;
        
        for( int a=0; a < 3; ++a ) {
            vertexes[indexes[i+a]].tangent += tangent;
            vertexes[indexes[i+a]].bitangent += bitangent;
        }
    }
    
    for( Vertex &vertex : vertexes ) {
        glm::vec3 &n = vertex.normal;
        glm::vec3 &t = vertex.tangent;
        glm::vec3 &b = vertex.bitangent;

        t = glm::normalize(t - n * glm::dot(n, t));
        b = glm::normalize( b );

        if (glm::dot(glm::cross(n, t), b) < 0.0f){
            t = t * -1.0f;
        }
    }
}

SharedPtr<Mesh> uploadMeshToGpu( const std::vector<Mesh::Vertex> &vertexes, 
                                 const std::vector<GLuint> &indexes,
                                 const std::vector<SubMesh> &submeshes,
                                 const BoundingSphere &bounds )
{
    typedef Mesh::Vertex Vertex;
    
    SharedPtr<GpuBuffer> vertexBuffer = GpuBuffer::CreateBuffer( 
                                            BufferType::Vertexes,
                                            sizeof(Vertex) * vertexes.size(),
                                            BufferUsage::WriteOnly, BufferUpdate::Static 
                                        );
    
    SharedPtr<GpuBuffer> indexBuffer = GpuBuffer::CreateBuffer( 
                                            BufferType::Indexes,
                                            sizeof(GLuint) * indexes.size(),
                                            BufferUsage::WriteOnly, BufferUpdate::Static 
                                       );
    vertexBuffer->setContent( vertexes.data(), vertexes.size() );
    indexBuffer->setContent( indexes.data(), indexes.size() );
    
        SharedPtr<VertexArrayObject> vao = makeSharedPtr<VertexArrayObject>();
    vao->bindVAO();
    vertexBuffer->bindBuffer();
    
    GLuint loc;
    loc = getDefaultAttributeLocation( DefaultAttributeLocations::Position );
    vao->setVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position) );
    
    loc = getDefaultAttributeLocation( DefaultAttributeLocations::Normal );
    vao->setVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal) );
    
    loc = getDefaultAttributeLocation( DefaultAttributeLocations::Tangent );
    vao->setVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, tangent) );
    
    loc = getDefaultAttributeLocation( DefaultAttributeLocations::Bitangent );
    vao->setVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, bitangent) );
    
    loc = getDefaultAttributeLocation( DefaultAttributeLocations::Texcoord );
    vao->setVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texcoord) );
    
    vao->unbindVAO();
    vertexBuffer->unbindBuffer();
    
    return makeSharedPtr<Mesh>( vao, vertexBuffer, indexBuffer, submeshes, bounds );
}
