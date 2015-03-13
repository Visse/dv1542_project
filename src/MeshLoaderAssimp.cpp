#include "MeshLoaderAssimp.h"
#include "StringUtils.h"
#include "GpuBuffer.h"
#include "Mesh.h"

#include "GLinclude.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <cassert>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/common.hpp>
#include <glm/geometric.hpp>

void countVertexesAndIndexes( const aiScene *scene, int &vertexCount, int &indexCount );

SharedPtr<Mesh> loadMesh( const aiScene *scene );
SharedPtr<Mesh> loadAnimatedMesh( const aiScene *scene );

template< typename Vertex >
BoundingSphere calculateBounds( Vertex *vertexes, int count );


SharedPtr<Mesh> MeshLoaderAssimp::loadFile( const std::string &filename )
{
    Assimp::Importer importer;
    
    const aiScene *scene = importer.ReadFile( filename, aiProcessPreset_TargetRealtime_Quality );
    
    if( !scene ) {
        throw std::runtime_error(StringUtils::strjoin("Failed to load mesh, error: ",importer.GetErrorString()));
    }
    if( !scene->HasMeshes() ) {
        return SharedPtr<Mesh>();
    }
    
    
    if( scene->HasAnimations() ) {
        return loadAnimatedMesh( scene );
    }
    else {
        return loadMesh( scene );
    }
}

void countVertexesAndIndexes( const aiScene *scene, int &vertexCount, int &indexCount )
{
    for( unsigned int i=0; i < scene->mNumMeshes; ++i ) {
        const aiMesh *mesh = scene->mMeshes[i];
        vertexCount += mesh->mNumVertices;
        indexCount += mesh->mNumFaces*3; // we know that every face is a triangle
    }
}

template< typename Vertex >
BoundingSphere calculateBounds( Vertex *vertexes, int count )
{
    glm::vec3 min = vertexes[0].position, max = vertexes[0].position;
    
    for( int i=0; i < count; ++i ) {
        min = glm::min( min, vertexes[i].position );
        max = glm::max( max, vertexes[i].position );
    }
    
    glm::vec3 center = (min + max) / 2.f;
    float radius = 0.f;
    
    for( int i=0; i < count; ++i ) {
        radius = glm::max( radius, glm::distance(center,vertexes[i].position) );
    }
    
    return BoundingSphere( center, radius );
}


glm::vec2 toGlm( const aiVector2D &v ) {
    return glm::vec2(v.x,v.y);
}

glm::vec3 toGlm( const aiVector3D &v ) {
    return glm::vec3(v.x,v.y,v.z);
}

SharedPtr<Mesh> loadMesh( const aiScene *scene )
{
    int vertexCount = 0;
    int indexCount = 0;
    
    countVertexesAndIndexes( scene, vertexCount, indexCount );
    
    struct Vertex {
        glm::vec3 position,
                  normal,
                  tangent,
                  bitangent;
        glm::vec2 texcoord;
    };
        
    SharedPtr<GpuBuffer> vertexBuffer = GpuBuffer::CreateBuffer( BufferType::Vertexes, sizeof(Vertex)*vertexCount, BufferUsage::WriteOnly, BufferUpdate::Static );
    SharedPtr<GpuBuffer> indexBuffer = GpuBuffer::CreateBuffer( BufferType::Indexes, sizeof(GLuint)*indexCount, BufferUsage::WriteOnly, BufferUpdate::Static );
    
    Vertex *vertexes = vertexBuffer->mapBuffer<Vertex>( BufferUsage::WriteOnly );
    GLuint *indexes = indexBuffer->mapBuffer<GLuint>( BufferUsage::WriteOnly );
    
    std::vector<SubMesh> submeshes;
    submeshes.resize( scene->mNumMeshes );
    
    Vertex *firstVertex = vertexes;
    
    size_t curVertex = 0, curIndex = 0;
    for( unsigned int i=0; i < scene->mNumMeshes; ++i ) {
        const aiMesh *mesh = scene->mMeshes[i];
        
        size_t startVertex = curVertex;
        for( unsigned int j=0; j < mesh->mNumVertices; ++j ) {
            vertexes->position  = toGlm(mesh->mVertices[j]);
            vertexes->normal    = toGlm(mesh->mNormals[j]);
            vertexes->tangent   = toGlm(mesh->mTangents[j]);
            vertexes->bitangent = toGlm(mesh->mBitangents[j]);
            vertexes->texcoord  = glm::vec2(toGlm(mesh->mTextureCoords[0][j]));
            
            curVertex++;
            vertexes++;
        }
        
        size_t indexCount = 0;
        for( unsigned int j=0; j < mesh->mNumFaces; ++j ) {
            const aiFace &face = mesh->mFaces[j];
            assert( face.mNumIndices == 3 );
            
            indexes[0] = face.mIndices[0] + startVertex;
            indexes[1] = face.mIndices[1] + startVertex;
            indexes[2] = face.mIndices[2] + startVertex;
            
            indexes += 3;
            indexCount += 3;
        }
        
        SubMesh &submesh = submeshes[i];
            submesh.vertexStart = curIndex;
            submesh.vertexCount = indexCount;
            
        curIndex += indexCount;
    }
    
    BoundingSphere bounds = calculateBounds( firstVertex, vertexCount );
    
    vertexBuffer->unmapBuffer();
    indexBuffer->unmapBuffer();
    
    SharedPtr<VertexArrayObject> vao = makeSharedPtr<VertexArrayObject>();
    
    vao->bindVAO();
    vertexBuffer->bindBuffer();
    
    vao->setVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position) );
    vao->setVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal) );
    vao->setVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texcoord) );
    
    vao->setVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, tangent) );
    vao->setVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, bitangent) );
    
    vao->unbindVAO();
    
    return makeSharedPtr<Mesh>( vao, vertexBuffer, indexBuffer, submeshes, bounds );
}


SharedPtr<Mesh> loadAnimatedMesh( const aiScene *scene )
{
    return SharedPtr<Mesh>();
}





 