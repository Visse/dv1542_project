#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <deque>
#include <string>
#include <unordered_map>
#include <vector>

#include "SharedPtr.h"
#include "Mesh.h"

class ResourceManager;

class MeshLoaderWavefront
{
public:
    struct Vertex {
        glm::vec3 position, 
                  normal;
        glm::vec2 texcoord;
    };
    
public:
    MeshLoaderWavefront( ResourceManager *resourceMgr );
    
    void loadFile( const std::string &filename );
    
    const std::vector<Vertex>& getVertexes() const {
        return mMeshInfo.vertexes;
    }
    const std::vector<unsigned int>& getIndexes() const {
        return mMeshInfo.indexes;
    }
    const std::vector<SubMesh>& getSubmeshes() const {
        return mMeshInfo.submeshes;
    }
    const BoundingSphere& getBounds() const {
        return mMeshInfo.bounds;
    }
    
private:
    struct Face {
        int position[3],
            normal[3],
            texcoord[3];
            
        glm::vec3 tangent, bitangent;
    };
    
private: // parser helper functions
    void parseLine( const std::string &line );
    void parseVertexPosition( const std::string &line );
    void parseVertexTexcoord( const std::string &line );
    void parseVertexNormal( const std::string &line );
    void parseFace( const std::string &line );
    
    bool parseVec2( const std::string &line, glm::vec2 &vec );
    bool parseVec3( const std::string &line, glm::vec3 &vec );
    bool parseVertex( const std::string &str, int index, Face &face );
    
private: // mesh building helping functions
    void validateFaces();
    void sortFaces();
    void buildMesh();
    void calculateBounds();
    
        
    struct VertexHash {
        size_t operator () ( const Vertex &v ) const;
    };
    struct VertexEqual {
        bool operator () ( const Vertex &v1, const Vertex &v2 ) const;
    };

private:
    ResourceManager *mResourceMgr;
    struct {
        int currentLine;
        
        std::deque<glm::vec3> positions,
                              normals,
                              tangents,
                              bitangents;
        std::deque<glm::vec2> texcoords;
        std::deque<Face> faces;
    } mParseInfo;
    
    struct {
        std::unordered_map<Vertex, int, VertexHash, VertexEqual> vertexLookup;
        
        std::vector<Vertex> vertexes;
        std::vector<unsigned int> indexes;
        
        std::vector<SubMesh> submeshes;
        
        BoundingSphere bounds;
    } mMeshInfo;
    
};