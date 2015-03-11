#include "MeshLoaderWavefront.h"
#include "StringUtils.h"
#include "ResourceManager.h"

#include <fstream>

#include <glm/vec4.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/epsilon.hpp>


MeshLoaderWavefront::MeshLoaderWavefront( ResourceManager *resourceMgr ) :
    mResourceMgr(resourceMgr)
{
}

void MeshLoaderWavefront::loadFile( const std::string &filename )
{
    std::ifstream file( filename );
    std::string line;
    
    mParseInfo.currentLine = 1;
    mParseInfo.positions.clear();
    mParseInfo.normals.clear();
    mParseInfo.texcoords.clear();
    mParseInfo.faces.clear();
    
    while( std::getline(file,line) ) {
        parseLine( StringUtils::stripSpaces(line) );
        mParseInfo.currentLine++;
    }
    
    validateFaces();
    sortFaces();
    buildMesh();
    calculateBounds();
}

void MeshLoaderWavefront::parseLine( const std::string &line )
{
    if( line.empty() ) return;
    if( StringUtils::startsWith(line, "v ") ) {
        parseVertexPosition( line  );
    }
    else if( StringUtils::startsWith(line,"vt ") ) {
        parseVertexTexcoord( line );
    }
    else if( StringUtils::startsWith(line,"vn ") ) {
        parseVertexNormal( line );
    }
    else if( StringUtils::startsWith(line,"f ") ) {
        parseFace( line );
    }
    else if( StringUtils::startsWith(line,"usemtl ") ) {} // use material, unsupported
    else if( StringUtils::startsWith(line,"#") ) {} // comment
    else if( StringUtils::startsWith(line,"s ") ) {} // smooth, unsupported
    else if( StringUtils::startsWith(line,"g ") ) {} // group
    else if( StringUtils::startsWith(line,"o ") ) {} // named object
    else if( StringUtils::startsWith(line,"mtllib ") ) {} // load material, unsupported
    else {
        throw std::runtime_error( StringUtils::strjoin("Invalid line(",mParseInfo.currentLine,")! \"",line,"\"!") );
    }
}

void MeshLoaderWavefront::parseVertexPosition( const std::string &line )
{
    glm::vec3 pos;
    if( !parseVec3(line, pos) ) {
        throw std::runtime_error( StringUtils::strjoin("Invalid position!, expected format is \"v x y z\", line ",mParseInfo.currentLine," says: \"",line,"\".") );
    }
    mParseInfo.positions.push_back( pos );
}

void MeshLoaderWavefront::parseVertexTexcoord( const std::string &line )
{   
    glm::vec2 texcoord;
    if( !parseVec2(line, texcoord) ) {
        throw std::runtime_error( StringUtils::strjoin("Invalid texcoord!, expected format is \"vt x y\", line ",mParseInfo.currentLine," says: \"",line,"\".") );
    }
    // wavefront object files stores them in dx format :/
    // aka, (0,0) is top-left, but we want (0,0) at bottom-left
    texcoord.y = 1.f - texcoord.y;
    mParseInfo.texcoords.push_back( texcoord );
}

void MeshLoaderWavefront::parseVertexNormal( const std::string &line )
{
    glm::vec3 normal;
    if( !parseVec3(line, normal) ) {
        throw std::runtime_error( StringUtils::strjoin("Invalid normal!, expected format is \"vn x y z\", line ",mParseInfo.currentLine," says: \"",line,"\".") );
    }
    mParseInfo.normals.push_back( normal );
}

void MeshLoaderWavefront::parseFace( const std::string &line )
{
    auto vertexes = StringUtils::split( line, std::vector<std::string>{" "} );
    if( vertexes.size() < 4 ) { // f [v1] [v2] [v3]
        throw std::runtime_error( StringUtils::strjoin("Invalid face!, expected format is \"f [v1] [v2] [v3]\", line ",mParseInfo.currentLine," says: \"",line,"\".") );
    }
    auto iter = vertexes.begin(), end = vertexes.end();
    int index = 0;
    Face face;
    for( ++iter; iter != end; ++iter, ++index ) {
        if( !parseVertex(*iter, index, face) ) {
            throw std::runtime_error( StringUtils::strjoin("Invalid face vertex!, expected format is \"[position]/[texcoord]/[normal]\", line ",mParseInfo.currentLine," says: \"",line,"\".") );
        }
        if( index == 2 ) {
            mParseInfo.faces.push_back( face );
            
            // for faces wiht more than 3 vertexes, we split it up using a triangle fan
            // aka the first triangle have the vertexes [0][1][2], the second [0][2][3], and so fourth
            face.position[1] = face.position[2];
            face.normal[1] = face.normal[2];
            face.texcoord[1] = face.texcoord[2];
            
            index--;
        }
    }
}

bool MeshLoaderWavefront::parseVec2( const std::string &line, glm::vec2 &vec )
{
    return std::sscanf(line.c_str(), "%*s %f %f", &vec.x, &vec.y) == 2;
}

bool MeshLoaderWavefront::parseVec3( const std::string &line, glm::vec3 &vec )
{
    return std::sscanf(line.c_str(), "%*s %f %f %f", &vec.x, &vec.y, &vec.z) == 3;
}

bool MeshLoaderWavefront::parseVertex( const std::string &str, int index, MeshLoaderWavefront::Face &face )
{
    int pos, tex = 0, norm = 0;
    int type = std::count( str.begin(), str.end(), '/' );
    if( type < 0 || type > 2 ) return false;
    
    switch( type ) {
    case( 0 ): 
        if( std::sscanf(str.c_str(), "%i", &pos) != 1 ) {
            return false;
        }
        break;
    case( 1 ):
        if( std::sscanf(str.c_str(), "%i/%i", &pos, &tex) != 2 ) {
            return false;
        }
        break;
    case( 2 ): 
        // we have two valid cases when there is two '/', (1) is 'p/t/n' and (2) is 'p//n'
        if( std::sscanf(str.c_str(), "%i/%i/%i",&pos, &tex, &norm) == 3 ) {
            break;
        }
        tex = 0;
        if( std::sscanf(str.c_str(), "%i//%i", &pos, &norm) == 2 ) {
            break;
        }
        return false;
    }
    
    face.position[index] = pos-1;
    face.texcoord[index] = tex-1;
    face.normal[index]   = norm-1;
    
    return true;
}

void MeshLoaderWavefront::validateFaces()
{    
    int faceIndex = 0;
    for( const Face &face : mParseInfo.faces ) {
        for( int i=0; i < 3; ++i ) {
            int pos = face.position[i],
                tex = face.texcoord[i],
                norm = face.normal[i];
            
            if( pos < 0 || (size_t)pos >= mParseInfo.positions.size() ) {
                throw std::runtime_error( StringUtils::strjoin("Face ",faceIndex," has a invalid position index(",pos,")!") );
            }
            if( tex >= 0 && (size_t)tex >= mParseInfo.texcoords.size() ) {
                throw std::runtime_error( StringUtils::strjoin("Face ",faceIndex," has a invalid texcoord index(",tex,")!") );
            }
            if( norm >= 0 && (size_t)norm >= mParseInfo.normals.size() ) {
                throw std::runtime_error( StringUtils::strjoin("Face ",faceIndex," has a invalid normal index(",norm,")!") );
            }
        }
        faceIndex++;
    }
}

void MeshLoaderWavefront::sortFaces()
{
}

void MeshLoaderWavefront::buildMesh()
{
    if( mParseInfo.faces.empty() )  return;
    
    size_t startIndex = 0, vertexCount = 0;
    for( const Face &face : mParseInfo.faces ) {
        for( int i=0; i < 3; ++i ) {
            Vertex vertex;
                vertex.position = mParseInfo.positions.at(face.position[i]);
            if( face.texcoord[i] >= 0 ) {
                vertex.texcoord = mParseInfo.texcoords.at(face.texcoord[i]);
            }
            if( face.normal[i] >= 0 ) {
                vertex.normal = mParseInfo.normals.at(face.normal[i]);
            }
            
            int index;
            auto iter = mMeshInfo.vertexLookup.find( vertex );
            if( iter != mMeshInfo.vertexLookup.end() ) {
                index = iter->second;
            }
            else {
                index = mMeshInfo.vertexes.size();
                mMeshInfo.vertexes.push_back( vertex );
                mMeshInfo.vertexLookup.emplace( vertex, index );
            }
            
            mMeshInfo.indexes.push_back( index );
            vertexCount++;
        }
    }
    
    SubMesh subMesh;
        subMesh.vertexCount = vertexCount;
        subMesh.vertexStart = startIndex;
    mMeshInfo.submeshes.push_back( subMesh );
}


static const float VERTEX_EPSILON = 1e-4f;

// floor to VERTEX_EPSILON
inline glm::vec4 floor( const glm::vec4 &v ) {
    return v - glm::mod(v,glm::vec4(VERTEX_EPSILON));
}
 
size_t MeshLoaderWavefront::VertexHash::operator()( const MeshLoaderWavefront::Vertex &v ) const
{
    /// @todo figure out how effective this hash is :/
    
    size_t pos = glm::packHalf4x16( floor(glm::vec4(v.position,0.f)) );
    size_t norm = glm::packHalf4x16( floor(glm::vec4(v.normal,0.f)) );
    size_t tex = glm::packHalf4x16( floor(glm::vec4(v.texcoord,0.f,0.f)) );
    
    size_t hash = pos;
    hash ^= (norm&0xFF) << 16 | (norm&0xFF) >> 16;
    hash ^= tex;
    
    return hash;
}

bool MeshLoaderWavefront::VertexEqual::operator()( const MeshLoaderWavefront::Vertex &v1, const MeshLoaderWavefront::Vertex &v2 ) const
{
    if( !glm::all(glm::epsilonEqual(v1.position,v2.position,VERTEX_EPSILON)) ) return false;
    if( !glm::all(glm::epsilonEqual(v1.normal, v2.normal, VERTEX_EPSILON)) ) return false;
    if( !glm::all(glm::epsilonEqual(v1.texcoord, v2.texcoord, VERTEX_EPSILON)) ) return false;
    return true;
}

void MeshLoaderWavefront::calculateBounds()
{
    if( mMeshInfo.vertexes.empty() ) {
        return;
    }
    
    glm::vec3 min = mMeshInfo.vertexes[0].position, max = mMeshInfo.vertexes[0].position;
    
    for( const Vertex &v : mMeshInfo.vertexes ) {
        min = glm::min( min, v.position );
        max = glm::max( max, v.position );
    }
    
    glm::vec3 center = (min + max) / 2.f;
    float radius = 0.f;
    
    for( const Vertex &v : mMeshInfo.vertexes ) {
        radius = glm::max( radius, glm::distance(center,v.position) );
    }
    
    mMeshInfo.bounds = BoundingSphere( center, radius );
}
