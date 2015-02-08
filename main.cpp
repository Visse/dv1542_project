#include "Root.h"

#include "FreeImage.h"
#include <iostream>
void freeImageOutput( FREE_IMAGE_FORMAT fif, const char *msg )
{
    std::clog << "[FreeImage] " << msg << std::endl;
}

int main(int argc, char **argv) 
{

    FreeImage_SetOutputMessage( freeImageOutput );
    
    Root root;
    if( !root.init() ) {
        return -1;
    }
    root.run();
    root.destroy();
    
    return 0;
}

