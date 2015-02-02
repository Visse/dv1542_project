#include "FileUtils.h"

#include <fstream>
#include <boost/filesystem.hpp>

namespace FileUtils 
{
    bool isFile( const std::string &filename )
    {
        return boost::filesystem::is_regular_file(filename);
    }
    
    bool isDirectory( const std::string &directory )
    {
        return boost::filesystem::is_directory(directory);
    }

    
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

        return std::move ( content );
    }
    
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
    }

}