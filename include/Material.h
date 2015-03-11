#pragma once

#include "SharedPtr.h"

class Texture;

struct DeferredMaterial 
{
    SharedPtr<Texture> diffuseTexture,
                       normalMap;
};





