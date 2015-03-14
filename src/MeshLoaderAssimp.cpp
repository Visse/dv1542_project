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
#include <map>

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

template< typename Vertex, typename Callable >
void buildMesh( const aiScene *scene, std::vector<SubMesh> &submeshes, SharedPtr<GpuBuffer> &vertexBuffer,
                                                                 SharedPtr<GpuBuffer> &indexBuffer,
                                                                 BoundingSphere &bounds, Callable parseVertex )
{
    int vertexCount = 0,
        indexCount = 0;
                 
    countVertexesAndIndexes( scene, vertexCount, indexCount );
    
    vertexBuffer = GpuBuffer::CreateBuffer( BufferType::Vertexes, sizeof(Vertex) * vertexCount, BufferUsage::WriteOnly, BufferUpdate::Static );
    indexBuffer = GpuBuffer::CreateBuffer( BufferType::Indexes, sizeof(GLuint) * indexCount, BufferUsage::WriteOnly, BufferUpdate::Static );
    
    Vertex *vertexes = vertexBuffer->mapBuffer<Vertex>( BufferUsage::WriteOnly );
    GLuint *indexes = indexBuffer->mapBuffer<GLuint>( BufferUsage::WriteOnly );
    
    unsigned int curVertex = 0, curIndex = 0;
    for( unsigned int i=0; i < scene->mNumMeshes; ++i ) {
        const aiMesh *mesh = scene->mMeshes[i];
        
        unsigned int startVertex = curVertex;
        for( unsigned int j=0; j < mesh->mNumVertices; ++j ) {
            parseVertex( j, mesh, vertexes[startVertex+j] );
        }
        curVertex += mesh->mNumVertices;
        
        unsigned int startIndex = curIndex;
        for( unsigned int j=0; j < mesh->mNumFaces; ++j ) {
            const aiFace *face = mesh->mFaces+j;
            assert( face->mNumIndices == 3 );
            
            for( unsigned int g=0; g < 3; ++g ) {
                indexes[startIndex+j*3+g] = face->mIndices[g] + startVertex;
            }
        }
        
        curIndex += mesh->mNumFaces * 3;
        
        SubMesh submesh;
            submesh.vertexStart = startIndex;
            submesh.vertexCount = mesh->mNumFaces * 3;
            
        submeshes.push_back( submesh );
    }
    
    bounds = calculateBounds( vertexes, vertexCount );
    
    indexBuffer->unmapBuffer();
    vertexBuffer->unmapBuffer();
}

glm::vec2 toGlm( const aiVector2D &v ) {
    return glm::vec2(v.x,v.y);
}

glm::vec3 toGlm( const aiVector3D &v ) {
    return glm::vec3(v.x,v.y,v.z);
}

SharedPtr<Mesh> loadMesh( const aiScene *scene )
{
    struct Vertex {
        glm::vec3 position,
                  normal,
                  tangent,
                  bitangent;
        glm::vec2 texcoord;
    };
    
    SharedPtr<GpuBuffer> vertexBuffer;
    SharedPtr<GpuBuffer> indexBuffer;
    BoundingSphere bounds;
    
    std::vector<SubMesh> submeshes;
    submeshes.reserve( scene->mNumMeshes );
    
    buildMesh<Vertex>( scene, submeshes, vertexBuffer, indexBuffer, bounds, 
        []( unsigned int i, const aiMesh *mesh, Vertex &vertex ) {
            vertex.position  = toGlm(mesh->mVertices[i]);
            vertex.normal    = toGlm(mesh->mNormals[i]);
            vertex.tangent   = toGlm(mesh->mTangents[i]);
            vertex.bitangent = toGlm(mesh->mBitangents[i]);
            vertex.texcoord  = glm::vec2(toGlm(mesh->mTextureCoords[0][i]));
        }
    );
    
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
    struct Vertex {
        glm::vec3 position,
                  normal,
                  tangent,
                  bitangent;
        glm::vec2 texcoord;
        uint8_t bones[4];
        float weights[4];
    };
    
    SharedPtr<GpuBuffer> vertexBuffer;
    SharedPtr<GpuBuffer> indexBuffer;
    BoundingSphere bounds;
    
    std::vector<SubMesh> submeshes;
    submeshes.reserve( scene->mNumMeshes );
        
    static const unsigned int NO_BONE = 0;
    
    struct BoneWeight {
        unsigned int bone = NO_BONE;
        float weight;
    };
    
    typedef std::array<BoneWeight,4> BoneWeightArray;
    typedef std::map<unsigned int, BoneWeightArray> BoneWeightMap;
    std::map<const aiMesh*, BoneWeightMap> vertexBoneLookup;
    
    
    auto insertWeight = []( BoneWeightArray &array, BoneWeight &weight ) {
        int pos = 0;
        for( ; pos < 4; ++pos ) {
            if( array[pos].bone == NO_BONE || array[pos].weight <= weight.weight ) break;
        }
        if( pos == 4 ) return;
        
        for( int i=2; i > pos; --i ) {
            array[i+1] = array[i];
        }
        array[pos] = weight;
    };
    
    // construct vertex-bone lookup
    for( unsigned int i=0; i < scene->mNumMeshes; ++i ) {
        const aiMesh *mesh = scene->mMeshes[i];
        BoneWeightMap &weights = vertexBoneLookup[mesh];
        
        for( unsigned int j=0; j < mesh->mNumBones; ++j ) {
            const aiBone *bone = mesh->mBones[j];
            
            for( unsigned int g=0; g < bone->mNumWeights; ++g ) {
                const aiVertexWeight &vertexWeight = bone->mWeights[g];
                BoneWeightArray &weightArray = weights[vertexWeight.mVertexId];
                BoneWeight weight;
                    weight.bone = j+1;
                    weight.weight = vertexWeight.mWeight;
                    
                insertWeight( weightArray, weight );
            }
        }
    }
    
    buildMesh<Vertex>( scene, submeshes, vertexBuffer, indexBuffer, bounds, 
        [&]( unsigned int i, const aiMesh *mesh, Vertex &vertex ) {
            vertex.position  = toGlm(mesh->mVertices[i]);
            vertex.normal    = toGlm(mesh->mNormals[i]);
            vertex.tangent   = toGlm(mesh->mTangents[i]);
            vertex.bitangent = toGlm(mesh->mBitangents[i]);
            vertex.texcoord  = glm::vec2(toGlm(mesh->mTextureCoords[0][i]));
            
            BoneWeightMap &weightMap = vertexBoneLookup[mesh];
            BoneWeightArray &weights = weightMap[i];
            
            for( int i=0; i < 4; ++i ) {
                vertex.bones[i] = weights[i].bone;
                vertex.weights[i] = weights[i].weight;
            }
        }
    );
    
    
    SharedPtr<VertexArrayObject> vao = makeSharedPtr<VertexArrayObject>();
    
    vao->bindVAO();
    vertexBuffer->bindBuffer();
    
    vao->setVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position) );
    vao->setVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal) );
    vao->setVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texcoord) );
    
    vao->setVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, tangent) );
    vao->setVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, bitangent) );
    
    vao->setVertexAttribPointer( 5, 4, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Vertex), offsetof(Vertex, bones) );
    vao->setVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, weights) );
    
    vao->unbindVAO();
    
    return makeSharedPtr<Mesh>( vao, vertexBuffer, indexBuffer, submeshes, bounds );
}

