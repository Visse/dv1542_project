#pragma once

#include <GLTypes.h>


enum class DefaultUniformBlockLocations {
    SceneInfo = 0,
    Material  = 1,
    
    
    COUNT
};

enum class DefaultAttributeLocations {
    Position = 0,
    Normal   = 1,
    Texcoord = 2,
    Tangent   = 3,
    Bitangent = 4,
    
    COUNT
};

enum class DefaultOutputLocations {
    Diffuse  = 0,
    Normal   = 1,
    Specular = 2,
    
    COUNT
};

inline GLuint getDefaultUniformBlockBinding( DefaultUniformBlockLocations uniform ) {
    return static_cast<GLuint>(uniform);
}
inline GLuint getDefaultAttributeLocation( DefaultAttributeLocations attribute ) {
    return static_cast<GLuint>(attribute);
}
inline GLuint getDefaultOutputLocation( DefaultOutputLocations output ) {
    return static_cast<GLuint>(output);
}

const char* getDefaultUniformBlockName( DefaultUniformBlockLocations uniform );
const char* getDefaultAttributeName( DefaultAttributeLocations attribute );
const char* getDefaultOutputName( DefaultOutputLocations output );