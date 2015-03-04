#include "DefaultGpuProgramLocations.h"


#include <cassert>

static const char* UNIFORM_BLOCK_NAME_LOOKUP[static_cast<int>(DefaultUniformBlockLocations::COUNT)] = 
{
    "SceneInfo",
    "Material"
};

static const char* ATTRIBUTE_NAME_LOOKUP[static_cast<int>(DefaultAttributeLocations::COUNT)] =
{
    "Position",
    "Normal",
    "Texcoord",
    "Tangent",
    "Bitangent"
};

static const char* OUTPUT_NAME_LOOKUP[static_cast<int>(DefaultOutputLocations::COUNT)] = 
{
    "Diffuse",
    "Normal"
};

static const char* GBUFFER_NAME_LOOKUP[static_cast<int>(DefaultGBufferBinding::COUNT)] =
{
    "Diffuse",
    "Normal",
    "Depth"
};
const char *getDefaultUniformBlockName( DefaultUniformBlockLocations uniform )
{
    int index = static_cast<int>(uniform);
    assert( index >= 0 && index < static_cast<int>(DefaultUniformBlockLocations::COUNT) );
    
    return UNIFORM_BLOCK_NAME_LOOKUP[index];
}

const char *getDefaultAttributeName( DefaultAttributeLocations attribute )
{
    int index = static_cast<int>(attribute);
    assert( index >= 0 && index < static_cast<int>(DefaultAttributeLocations::COUNT) );
    
    return ATTRIBUTE_NAME_LOOKUP[index];
}

const char *getDefaultOutputName( DefaultOutputLocations output )
{
    int index = static_cast<int>( output );
    assert( index >= 0 && index < static_cast<int>(DefaultOutputLocations::COUNT)  );
    
    return OUTPUT_NAME_LOOKUP[index];
}

const char* getDefaultGBufferName( DefaultGBufferBinding binding )
{
    int index = static_cast<int>( binding );
    assert( index >= 0 && index < static_cast<int>(DefaultGBufferBinding::COUNT) );
    
    return GBUFFER_NAME_LOOKUP[index];
}
