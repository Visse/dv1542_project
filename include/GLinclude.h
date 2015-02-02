#ifndef GLINCLUDE_H
#define GLINCLUDE_H

// help file to help our poor parser understand the all the glew macros :(

#ifndef IN_IDE_PARSER
// used then compiling
#include <GL/glew.h>
#include <GL/gl.h>
#else
// used then parsed by our IDE
// make gl prototypes avaliable
#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glew.h>

#error This part should only be used by the IDE
#endif

// helper macro to get the offset of a member in gl format.
#define gloffsetof( Type, Member ) reinterpret_cast<void*>(offsetof( Type, Member ))


#endif
