#pragma once

#include <string>

#include "SharedPtr.h"


class Mesh;

class MeshLoaderAssimp
{
public:
    SharedPtr<Mesh> loadFile( const std::string &filename );
    
};
