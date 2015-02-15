#include "FileUtils.h"

#include <fstream>

namespace FileUtils 
{
    std::string getFileContent(const std::string& filename)
    {
        std::fstream file ( filename );

        auto beg = file.tellg();
        file.seekg ( 0, std::ios::end );
        auto end = file.tellg();
        file.seekg ( beg );

        size_t size = end - beg;

        std::string content;
        content.resize ( size );
        file.read ( &content[0], size );
        // apperently since we opened the file in text mode,
        // we got a incorrect size from tellg :/,
        // so now we have to find out how many characters we 'really' read.
        size_t charactersRead = file.gcount();
        content.resize( charactersRead );

        return std::move ( content );
    }
}

#ifndef WIN32
#include <sys/stat.h>

namespace FileUtils 
{
    bool isFile( const std::string &filename )
    {
        struct stat buf;
        lstat( filename.c_str(), &buf );
        
        return S_ISREG(buf.st_mode);
    }
    
    bool isDirectory( const std::string &directory )
    {
        struct stat buf;
        lstat( directory.c_str(), &buf );
        
        return S_ISDIR(buf.st_mode);
    }
    
/*
    namespace filesystem {
        using namespace boost::filesystem;
    }
    std::vector<std::string> getFilesInDirectory(const std::string& directory)
    {
        std::vector<std::string> res;
        
        if( !filesystem::is_directory(directory) ) return res;
        
        filesystem::directory_iterator end, iter( directory );

        for( ; iter != end; ++iter )
        {
            if( filesystem::is_regular_file(*iter) )
            {
                filesystem::path path = iter->path();
                res.push_back( path.string() );
            }
        }
        
        return std::move(res);
    }*/

}


#else
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace FileUtils 
{
    bool isFile( const std::string &filename )
    {
        DWORD attributes = GetFileAttributes(filename.c_str());
        if( attributes == 0xFFFFFFFF ) {
            return false;
        }
        return attributes  == FILE_ATTRIBUTE_NORMAL;
    }
    
    bool isDirectory( const std::string &directory )
    {
        DWORD attributes = GetFileAttributes(directory.c_str());
        if( attributes == 0xFFFFFFFF ) {
            return false;
        }
        return attributes  == FILE_ATTRIBUTE_DIRECTORY;
    }

    /*
    std::vector<std::string> getFilesInDirectory(const std::string& directory)
    {
        std::vector<std::string> res;
        
        if( !isDirectory(directory) ) {
            return res;
        }
        
        return res;
    }*/
 }

#endif