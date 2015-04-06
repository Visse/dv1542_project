#pragma once

#include <vector>

#include "SharedPtr.h"
#include "GLinclude.h"


// static const size_t DEFAULT_UNIFORM_BUFFER_SIZE = 524288, // 0.5 MB
static const size_t DEFAULT_UNIFORM_BUFFER_SIZE = 1024*128, // 128 KB
                    DEFAULT_UNIFORM_BUFFER_CLEAN_UP_TIME = 100; // to clean up unused buffers every 100 frame seems resonable


class UniformBufferAllocator {
public:
    UniformBufferAllocator( const UniformBufferAllocator& ) = delete;
    UniformBufferAllocator( UniformBufferAllocator&& ) = delete;
    UniformBufferAllocator& operator = ( const UniformBufferAllocator& ) = delete;
    UniformBufferAllocator& operator = ( UniformBufferAllocator&& ) = delete;
    
public:
    struct AllocationResult {
        GLuint buffer;
        size_t offset;
        void *memory;
    };
    
public:
    UniformBufferAllocator( size_t defaultBufferSize=DEFAULT_UNIFORM_BUFFER_SIZE, size_t unusedCleanupTime=DEFAULT_UNIFORM_BUFFER_CLEAN_UP_TIME ) :
        mDefaultBufferSize(defaultBufferSize),
        mUnusedCleanupTime(unusedCleanupTime)
    {
        GLint aligment;
        glGetIntegerv( GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &aligment );
        mAligment = aligment;
    }
    
    ~UniformBufferAllocator()
    {
        for( BufferInfo &info : mBuffers ) {
            if( info.mapped != nullptr ) {
                glBindBuffer( GL_UNIFORM_BUFFER, info.buffer );
                glUnmapBuffer( GL_UNIFORM_BUFFER );
            }
            glDeleteBuffers( 1, &info.buffer );
        }
    }
    
    AllocationResult getMemory( size_t size )
    {
        for( BufferInfo &info : mBuffers ) {
            if( info.mapped == nullptr && size < info.size ) {
                // if we are not mapped and we can hold a uniform of specified size
                // map this buffer.
                mapBuffer( info );
            }
            if( info.mapped && (info.offset+size) < info.size )  {
                // if we are mapped and can hold the uniform, allocate it :)
                return allocateFromBuffer( info, size );
            }
        }
        
        // we failed to allocate with existing buffers, create a new one
        size_t bufSize = std::max(mDefaultBufferSize,size);
        BufferInfo buffer;
            buffer.size = bufSize;
            buffer.mapped = nullptr;
            buffer.offset = 0;
            buffer.unused = 0;
        
        glGenBuffers( 1, &buffer.buffer );
        glBindBuffer( GL_UNIFORM_BUFFER, buffer.buffer );

        glBufferData( GL_UNIFORM_BUFFER, bufSize, NULL, GL_DYNAMIC_DRAW );
        mBuffers.push_back( buffer );
        
        BufferInfo &tmp = mBuffers.back();
        
        mapBuffer( tmp );
        return allocateFromBuffer( tmp, size );
    }
    
    size_t flushAndReset()
    {
        size_t memUsage = 0;
        for( BufferInfo &info : mBuffers ) 
        {
            if( info.mapped ) {
                glBindBuffer( GL_UNIFORM_BUFFER, info.buffer );
                glFlushMappedBufferRange( GL_UNIFORM_BUFFER, 0, info.offset );
                glUnmapBuffer( GL_UNIFORM_BUFFER );
                memUsage += info.offset;
                
                info.mapped = nullptr;
                info.offset = 0;
            }
            else {
                // we didn't map this buffer... mark it as unused
                info.unused++;
            }
        }
        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        
        if( !mBuffers.empty() ) {
            // clean up 'dead' buffers, aka buffers that haven't been used for a time
            auto cur = mBuffers.begin(), end = mBuffers.end()-1;
            
            while( cur != end ) {
                if( cur->unused > mUnusedCleanupTime ) {
                    std::swap( *cur, *end );
                    end--;
                }
                else {
                    ++cur;
                }
            }
            
            mBuffers.erase( end+1, mBuffers.end() );
        }
        
        return memUsage;
    }
    
private:
    struct BufferInfo {
        GLuint buffer;
        size_t size;
        
        void *mapped;
        size_t offset, unused;
    };
    
private:
    void mapBuffer( BufferInfo &info ) 
    {
        glBindBuffer( GL_UNIFORM_BUFFER, info.buffer );
        info.mapped = glMapBufferRange( GL_UNIFORM_BUFFER, 0, info.size, 
                                        GL_MAP_WRITE_BIT |  // we are going to write to the mapped memory
                                        GL_MAP_INVALIDATE_BUFFER_BIT |  // we don't care of what is currently in there (we are going to overwrite it)
                                        GL_MAP_FLUSH_EXPLICIT_BIT ); // we will tell opengl what parts of the buffer we wrote to
        info.offset = 0;
        info.unused = 0;
        
    }
    AllocationResult allocateFromBuffer( BufferInfo &info, size_t size )
    {
        AllocationResult result;
            result.memory = (void*)((uintptr_t)(info.mapped) + info.offset);
            result.buffer = info.buffer;
            result.offset = info.offset;
        info.offset += size;
        
        // make sure that we are in correct aligment
        size_t correction = mAligment - info.offset % mAligment;
        // incase if we already where aligned.
        correction %= mAligment;
        info.offset += correction;
        
        return result;
    }
private:
    std::vector<BufferInfo> mBuffers;
    size_t mDefaultBufferSize, mUnusedCleanupTime;
    size_t mAligment;
};